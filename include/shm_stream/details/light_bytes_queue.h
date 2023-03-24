/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*!
 * \file
 * \brief Definition of queues of bytes without waiting (possibly lock-free and
 * wait-free).
 */
#pragma once

#include <algorithm>
#include <limits>
#include <type_traits>

#include <boost/atomic/ipc_atomic.hpp>
#include <boost/memory_order.hpp>
#include <fmt/format.h>

#include "shm_stream/bytes_view.h"
#include "shm_stream/c_interface/error_codes.h"
#include "shm_stream/common_types.h"
#include "shm_stream/details/atomic_index_pair.h"
#include "shm_stream/shm_stream_assert.h"
#include "shm_stream/shm_stream_exception.h"

namespace shm_stream {
namespace details {

/*!
 * \brief Class of writer of queues of bytes without waiting (possibly
 * lock-free and wait-free).
 *
 * \tparam AtomicType Type of atomic variables.
 *
 * \thread_safety All operation is safe if only one writer exists.
 */
template <typename AtomicType = boost::atomics::ipc_atomic<shm_stream_size_t>>
class light_bytes_queue_writer {
public:
    //! Type of the atomic variables.
    using atomic_type = AtomicType;

    static_assert(std::is_same<typename atomic_type::value_type,
                      shm_stream_size_t>::value,
        "Type of values in atomic variables must be equal to "
        "shm_stream_size_t.");

    /*!
     * \brief Get the maximum size of buffers.
     *
     * \return Size.
     */
    static constexpr shm_stream_size_t max_size() noexcept {
        return std::numeric_limits<shm_stream_size_t>::max() / 2U;
    }

    /*!
     * \brief Get the minimum size of buffers.
     *
     * \return Size.
     */
    static constexpr shm_stream_size_t min_size() noexcept { return 2U; }

    /*!
     * \brief Get whether this implementation is lock-free in the current
     * environment.
     *
     * \note When this implementation is lock-free, this implementation is also
     * wait-free.
     *
     * \retval true Lock free.
     * \retval false Not lock-free.
     */
    static constexpr bool is_always_lock_free() noexcept {
        return atomic_type::is_always_lock_free;
    }

    /*!
     * \brief Constructor.
     *
     * \param[in] atomic_indices Atomic variables of the indices of the next
     * bytes for the writer and the reader.
     * \param[in] buffer Buffer of data.
     */
    light_bytes_queue_writer(atomic_index_pair_view<atomic_type> atomic_indices,
        mutable_bytes_view buffer)
        : atomic_next_read_index_(&atomic_indices.reader()),
          atomic_next_write_index_(&atomic_indices.writer()),
          buffer_(buffer.data()),
          size_(buffer.size()),
          next_write_index_(0U),
          reserved_(0U) {
        SHM_STREAM_ASSERT(atomic_next_read_index_ != nullptr);
        SHM_STREAM_ASSERT(atomic_next_read_index_->load() < max_size());
        SHM_STREAM_ASSERT(atomic_next_write_index_ != nullptr);
        SHM_STREAM_ASSERT(atomic_next_write_index_->load() < max_size());
        SHM_STREAM_ASSERT(buffer_ != nullptr);

        if (size_ < min_size() || size_ > max_size()) {
            throw shm_stream_error(c_shm_stream_error_code_invalid_argument);
        }

        next_write_index_ =
            atomic_next_write_index_->load(boost::memory_order::relaxed);
    }

    // Prevent copy.
    light_bytes_queue_writer(const light_bytes_queue_writer&) = delete;
    auto operator=(const light_bytes_queue_writer&) = delete;

    //! Move constructor.
    light_bytes_queue_writer(light_bytes_queue_writer&&) noexcept = default;

    /*!
     * \brief Move assignment operator.
     *
     * \return This.
     */
    auto operator=(light_bytes_queue_writer&&) noexcept
        -> light_bytes_queue_writer& = default;

    //! Destructor.
    ~light_bytes_queue_writer() noexcept = default;

    /*!
     * \brief Get the size of the available bytes to write.
     *
     * \return Size of the available bytes to write.
     */
    [[nodiscard]] shm_stream_size_t available_size() const noexcept {
        shm_stream_size_t next_read_index =
            atomic_next_read_index_->load(boost::memory_order::relaxed);

        if (next_read_index <= next_write_index_) {
            next_read_index += size_;
            SHM_STREAM_ASSERT(next_read_index > next_write_index_);
        }
        return next_read_index - next_write_index_ - 1U;
    }

    /*!
     * \brief Try to reserve some bytes to write.
     *
     * \param[in] expected_size Expected number of bytes to reserve to write.
     * \return Buffer of the reserved bytes.
     *
     * \note This function tries to reserve given number of bytes, but a smaller
     * or empty buffer may be returned.
     * \note This function can return a buffer with a size smaller than the
     * return value of available_size function, because this queue is a circular
     * buffer and this function reserves continuous byte sequences from the
     * circular buffer.
     */
    [[nodiscard]] mutable_bytes_view try_reserve(
        shm_stream_size_t expected_size = max_size()) noexcept {
        const shm_stream_size_t next_read_index =
            atomic_next_read_index_->load(boost::memory_order::acquire);

        const shm_stream_size_t max_reservable_size =
            calc_reservable_size(next_read_index);
        reserved_ = std::min(expected_size, max_reservable_size);

        return mutable_bytes_view(buffer_ + next_write_index_, reserved_);
    }

    /*!
     * \brief Save written bytes as completed and ready to be read by a reader.
     *
     * \param[in] written_size Number of written bytes to save.
     */
    void commit(shm_stream_size_t written_size) noexcept {
        if (written_size == 0U) {
            return;
        }
        SHM_STREAM_ASSERT(written_size <= reserved_);

        next_write_index_ += written_size;
        if (next_write_index_ == size_) {
            next_write_index_ = 0;
        }
        SHM_STREAM_ASSERT(next_write_index_ < size_);

        atomic_next_write_index_->store(
            next_write_index_, boost::memory_order::release);

        reserved_ = 0U;
    }

private:
    /*!
     * \brief Calculate the number of reservable bytes.
     *
     * \param[in] next_read_index Value of atomic_next_read_index_.
     * \return Number of reservable bytes.
     */
    [[nodiscard]] shm_stream_size_t calc_reservable_size(
        shm_stream_size_t next_read_index) const noexcept {
        if (next_write_index_ < next_read_index) {
            return next_read_index - next_write_index_ - 1U;
        }
        if (next_read_index == 0U) {
            return size_ - next_write_index_ - 1U;
        }
        return size_ - next_write_index_;
    }

    //! Atomic variable of the index of the next byte to read.
    atomic_type* atomic_next_read_index_;

    //! Atomic variable of the index of the next byte to write.
    atomic_type* atomic_next_write_index_;

    //! Pointer to the buffer.
    char* buffer_;

    //! Size of the buffer.
    shm_stream_size_t size_;

    //! Index of the next byte to write.
    shm_stream_size_t next_write_index_;

    //! Number of bytes reserved to write currently.
    shm_stream_size_t reserved_;
};

/*!
 * \brief Class of reader of queues of bytes without waiting  (possibly
 * lock-free and wait-free).
 *
 * \tparam AtomicType Type of atomic variables.
 *
 * \thread_safety All operation is safe if only one writer exists.
 */
template <typename AtomicType = boost::atomics::ipc_atomic<shm_stream_size_t>>
class light_bytes_queue_reader {
public:
    //! Type of the atomic variables.
    using atomic_type = AtomicType;

    static_assert(std::is_same<typename atomic_type::value_type,
                      shm_stream_size_t>::value,
        "Type of values in atomic variables must be equal to "
        "shm_stream_size_t.");

    /*!
     * \brief Get the maximum size of buffers.
     *
     * \return Size.
     */
    static constexpr shm_stream_size_t max_size() noexcept {
        return std::numeric_limits<shm_stream_size_t>::max() / 2U;
    }

    /*!
     * \brief Get the minimum size of buffers.
     *
     * \return Size.
     */
    static constexpr shm_stream_size_t min_size() noexcept { return 2U; }

    /*!
     * \brief Get whether this implementation is lock-free in the current
     * environment.
     *
     * \note When this implementation is lock-free, this implementation is also
     * wait-free.
     *
     * \retval true Lock free.
     * \retval false Not lock-free.
     */
    static constexpr bool is_always_lock_free() noexcept {
        return atomic_type::is_always_lock_free;
    }

    /*!
     * \brief Constructor.
     *
     * \param[in] atomic_indices Atomic variables of the indices of the next
     * bytes for the writer and the reader.
     * \param[in] buffer Buffer of data.
     */
    light_bytes_queue_reader(
        atomic_index_pair_view<atomic_type> atomic_indices, bytes_view buffer)
        : atomic_next_read_index_(&atomic_indices.reader()),
          atomic_next_write_index_(&atomic_indices.writer()),
          buffer_(buffer.data()),
          size_(buffer.size()),
          next_read_index_(0U),
          reserved_(0U) {
        SHM_STREAM_ASSERT(atomic_next_read_index_ != nullptr);
        SHM_STREAM_ASSERT(atomic_next_read_index_->load() < max_size());
        SHM_STREAM_ASSERT(atomic_next_write_index_ != nullptr);
        SHM_STREAM_ASSERT(atomic_next_write_index_->load() < max_size());
        SHM_STREAM_ASSERT(buffer_ != nullptr);

        if (size_ < min_size() || size_ > max_size()) {
            throw shm_stream_error(c_shm_stream_error_code_invalid_argument);
        }

        next_read_index_ =
            atomic_next_read_index_->load(boost::memory_order::relaxed);
    }

    // Prevent copy.
    light_bytes_queue_reader(const light_bytes_queue_reader&) = delete;
    auto operator=(const light_bytes_queue_reader&) = delete;

    //! Move constructor.
    light_bytes_queue_reader(light_bytes_queue_reader&&) noexcept = default;

    /*!
     * \brief Move assignment operator.
     *
     * \return This.
     */
    auto operator=(light_bytes_queue_reader&&) noexcept
        -> light_bytes_queue_reader& = default;

    //! Destructor.
    ~light_bytes_queue_reader() noexcept = default;

    /*!
     * \brief Get the size of the available bytes to read.
     *
     * \return Size of the available bytes to read.
     */
    [[nodiscard]] shm_stream_size_t available_size() const noexcept {
        shm_stream_size_t next_write_index =
            atomic_next_write_index_->load(boost::memory_order::relaxed);

        if (next_write_index < next_read_index_) {
            next_write_index += size_;
        }
        SHM_STREAM_ASSERT(next_read_index_ <= next_write_index);

        return next_write_index - next_read_index_;
    }

    /*!
     * \brief Try to reserve some bytes to read.
     *
     * \param[in] expected_size Expected number of bytes to reserve to read.
     * \return Buffer of the reserved bytes.
     *
     * \note This function tries to reserve given number of bytes, but a smaller
     * or empty buffer may be returned.
     * \note This function can return a buffer with a size smaller than the
     * return value of available_size function, because this queue is a circular
     * buffer and this function reserves continuous byte sequences from the
     * circular buffer.
     */
    [[nodiscard]] bytes_view try_reserve(
        shm_stream_size_t expected_size = max_size()) noexcept {
        const shm_stream_size_t next_write_index =
            atomic_next_write_index_->load(boost::memory_order::acquire);

        const shm_stream_size_t max_reservable_size =
            calc_reservable_size(next_write_index);
        reserved_ = std::min(expected_size, max_reservable_size);

        return bytes_view(buffer_ + next_read_index_, reserved_);
    }

    /*!
     * \brief Set some bytes finished to read and ready to write by a writer.
     *
     * \param[in] read_size Number of bytes to set finished to read.
     */
    void commit(shm_stream_size_t read_size) noexcept {
        if (read_size == 0U) {
            return;
        }
        SHM_STREAM_ASSERT(read_size <= reserved_);

        next_read_index_ += read_size;
        if (next_read_index_ == size_) {
            next_read_index_ = 0;
        }
        SHM_STREAM_ASSERT(next_read_index_ < size_);

        atomic_next_read_index_->store(
            next_read_index_, boost::memory_order::release);

        reserved_ = 0U;
    }

private:
    /*!
     * \brief Calculate the number of reservable bytes.
     *
     * \param[in] next_write_index Value of atomic_next_write_index_.
     * \return Number of reservable bytes.
     */
    [[nodiscard]] shm_stream_size_t calc_reservable_size(
        shm_stream_size_t next_write_index) const noexcept {
        if (next_read_index_ <= next_write_index) {
            return next_write_index - next_read_index_;
        }
        return size_ - next_read_index_;
    }

    //! Atomic variable of the index of the next byte to read.
    atomic_type* atomic_next_read_index_;

    //! Atomic variable of the index of the next byte to write.
    atomic_type* atomic_next_write_index_;

    //! Pointer to the buffer.
    const char* buffer_;

    //! Size of the buffer.
    shm_stream_size_t size_;

    //! Index of the next byte to read.
    shm_stream_size_t next_read_index_;

    //! Number of bytes reserved to read currently.
    shm_stream_size_t reserved_;
};

}  // namespace details
}  // namespace shm_stream
