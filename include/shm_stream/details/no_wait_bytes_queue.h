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
 * \brief Definition of queues of bytes without waiting (possibly lock-free).
 */
#pragma once

#include <algorithm>
#include <type_traits>

#include <boost/atomic/ipc_atomic.hpp>
#include <boost/memory_order.hpp>
#include <fmt/format.h>

#include "shm_stream/bytes_view.h"
#include "shm_stream/common_types.h"
#include "shm_stream/details/atomic_index_pair.h"
#include "shm_stream/shm_stream_assert.h"
#include "shm_stream/shm_stream_exception.h"

namespace shm_stream {
namespace details {

/*!
 * \brief Class of writer of queues of bytes without waiting (possibly
 * lock-free).
 *
 * \tparam AtomicType Type of atomic variables.
 *
 * \thread_safety All operation is safe if only one writer exists.
 */
template <typename AtomicType = boost::atomics::ipc_atomic<shm_stream_size_t>>
class no_wait_bytes_queue_writer {
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
     * \brief Constructor.
     *
     * \param[in] atomic_indices Atomic variables of the indices of the next
     * bytes for the writer and the reader.
     * \param[in] buffer Pointer to the buffer of data.
     */
    no_wait_bytes_queue_writer(
        atomic_index_pair_view<atomic_type> atomic_indices,
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
            throw invalid_argument(fmt::format(
                "Invalid buffer size. (min: {}, max: {}, actual: {})",
                min_size(), max_size(), size_));
        }

        next_write_index_ =
            atomic_next_write_index_->load(boost::memory_order::relaxed);
    }

    // Prevent copy.
    no_wait_bytes_queue_writer(const no_wait_bytes_queue_writer&) = delete;
    auto operator=(const no_wait_bytes_queue_writer&) = delete;

    //! Move constructor.
    no_wait_bytes_queue_writer(no_wait_bytes_queue_writer&&) noexcept = default;

    /*!
     * \brief Move assignment operator.
     *
     * \return This.
     */
    auto operator=(no_wait_bytes_queue_writer&&) noexcept
        -> no_wait_bytes_queue_writer& = default;

    //! Destructor.
    ~no_wait_bytes_queue_writer() = default;

    /*!
     * \brief Get the size of the available bytes to write.
     *
     * \return Size of the available bytes to write.
     */
    [[nodiscard]] shm_stream_size_t available_size() const {
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
     */
    [[nodiscard]] mutable_bytes_view try_reserve(
        shm_stream_size_t expected_size) noexcept {
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
        shm_stream_size_t next_read_index) const {
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

}  // namespace details
}  // namespace shm_stream
