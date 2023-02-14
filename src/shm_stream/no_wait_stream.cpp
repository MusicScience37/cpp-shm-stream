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
 * \brief Implementation of streams of bytes without waiting (possibly
 * lock-free).
 */
#include "shm_stream/no_wait_stream.h"

#include <mutex>
#include <string>
#include <utility>

#include <boost/interprocess/creation_tags.hpp>
#include <boost/interprocess/detail/os_file_functions.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include "shm_stream/bytes_view.h"
#include "shm_stream/common_types.h"
#include "shm_stream/details/atomic_index_pair.h"
#include "shm_stream/details/cache_line_size.h"
#include "shm_stream/details/no_wait_bytes_queue.h"

namespace shm_stream {

namespace details {

/*!
 * \brief Header of the data shared between no_wait_stream_writer and
 * no_wait_stream_reader.
 */
struct no_wait_stream_header {
    //! Atomic variables of indices.
    alignas(cache_line_size()) details::atomic_index_pair<> indices{};

    //! Size of the buffer.
    alignas(cache_line_size()) shm_stream_size_t buffer_size{};
};

static_assert(sizeof(no_wait_stream_header) == 3U * cache_line_size(),
    "Unexpected size of no_wait_stream_header.");

/*!
 * \brief Data of no_wait_stream_writer and no_wait_stream_reader.
 */
struct no_wait_stream_data {
    //! Shared memory object.
    boost::interprocess::shared_memory_object shared_memory{};

    //! Mapped region.
    boost::interprocess::mapped_region mapped_region{};

    /*!
     * \brief Atomic variables of the indices of the next bytes for the writer
     * and the reader.
     */
    atomic_index_pair<>* atomic_indices{nullptr};

    //! Buffer of data.
    mutable_bytes_view buffer{nullptr, 0U};
};

/*!
 * \brief Get the name of the shared memory of a stream.
 *
 * \param[in] stream_name Name of the stream.
 * \return Name of the shared memory.
 */
[[nodiscard]] std::string no_wait_stream_shm_name(string_view stream_name) {
    return "shm_stream_no_wait_stream_data_" +
        std::string(stream_name.data(), stream_name.size());
}

/*!
 * \brief Get the name of the mutex of a stream.
 *
 * \param[in] stream_name Name of the stream.
 * \return Name of the mutex.
 */
[[nodiscard]] std::string no_wait_stream_mutex_name(string_view stream_name) {
    return "shm_stream_no_wait_stream_lock_" +
        std::string(stream_name.data(), stream_name.size());
}

/*!
 * \brief Prepare data of no_wait_stream_writer and no_wait_stream_reader.
 *
 * \param[in] name Name of the stream.
 * \param[in] buffer_size Size of the buffer.
 * \return Data.
 */
[[nodiscard]] no_wait_stream_data prepare_no_wait_stream_data(
    string_view name, shm_stream_size_t buffer_size) {
    no_wait_stream_data data{};

    const std::string data_shm_name = no_wait_stream_shm_name(name);
    const std::string mutex_name = no_wait_stream_mutex_name(name);
    boost::interprocess::named_mutex mutex{
        boost::interprocess::open_or_create, mutex_name.c_str()};
    std::unique_lock<boost::interprocess::named_mutex> lock(mutex);

    try {
        data.shared_memory = boost::interprocess::shared_memory_object(
            boost::interprocess::open_only, data_shm_name.c_str(),
            boost::interprocess::read_write);
    } catch (...) {
        // Shared memory doesn't exist, so create one.

        data.shared_memory = boost::interprocess::shared_memory_object(
            boost::interprocess::create_only, data_shm_name.c_str(),
            boost::interprocess::read_write);

        const boost::interprocess::offset_t data_size =
            static_cast<boost::interprocess::offset_t>(
                sizeof(no_wait_stream_header)) +
            static_cast<boost::interprocess::offset_t>(buffer_size);
        data.shared_memory.truncate(data_size);

        data.mapped_region = boost::interprocess::mapped_region(
            data.shared_memory, boost::interprocess::read_write);

        auto* header =
            new (data.mapped_region.get_address()) no_wait_stream_header();
        header->indices.writer() = 0U;
        header->indices.reader() = 0U;
        header->buffer_size = buffer_size;

        data.atomic_indices = &header->indices;
        data.buffer = mutable_bytes_view(
            static_cast<char*>(static_cast<void*>(header + 1)),
            header->buffer_size);

        return data;
    }

    data.mapped_region = boost::interprocess::mapped_region(
        data.shared_memory, boost::interprocess::read_write);

    auto* header =
        static_cast<no_wait_stream_header*>(data.mapped_region.get_address());
    data.atomic_indices = &header->indices;
    data.buffer =
        mutable_bytes_view(static_cast<char*>(static_cast<void*>(header + 1)),
            header->buffer_size);

    return data;
}

}  // namespace details

//! Type of the internal data.
struct no_wait_stream_writer::impl_type {
    //! Shared memory object.
    boost::interprocess::shared_memory_object shared_memory;

    //! Mapped region.
    boost::interprocess::mapped_region mapped_region;

    //! Writer.
    details::no_wait_bytes_queue_writer<> writer;

    /*!
     * \brief Constructor.
     *
     * \param[in] data Data.
     */
    explicit impl_type(details::no_wait_stream_data&& data)
        : shared_memory(std::move(data.shared_memory)),
          mapped_region(std::move(data.mapped_region)),
          writer(*data.atomic_indices, data.buffer) {}
};

no_wait_stream_writer::no_wait_stream_writer() : impl_(nullptr) {}

no_wait_stream_writer::~no_wait_stream_writer() noexcept { close(); }

void no_wait_stream_writer::open(
    string_view name, shm_stream_size_t buffer_size) {
    close();
    impl_ =
        new impl_type(details::prepare_no_wait_stream_data(name, buffer_size));
}

void no_wait_stream_writer::close() noexcept {
    if (impl_ == nullptr) {
        return;
    }
    delete impl_;
    impl_ = nullptr;
}

shm_stream_size_t no_wait_stream_writer::available_size() const noexcept {
    if (impl_ == nullptr) {
        return 0U;
    }
    return impl_->writer.available_size();
}

mutable_bytes_view no_wait_stream_writer::try_reserve(
    shm_stream_size_t expected_size) noexcept {
    if (impl_ == nullptr) {
        return mutable_bytes_view(nullptr, 0U);
    }
    return impl_->writer.try_reserve(expected_size);
}

mutable_bytes_view no_wait_stream_writer::try_reserve() noexcept {
    if (impl_ == nullptr) {
        return mutable_bytes_view(nullptr, 0U);
    }
    return impl_->writer.try_reserve();
}

void no_wait_stream_writer::commit(shm_stream_size_t written_size) noexcept {
    if (impl_ == nullptr) {
        return;
    }
    impl_->writer.commit(written_size);
}

//! Type of the internal data.
struct no_wait_stream_reader::impl_type {
    //! Shared memory object.
    boost::interprocess::shared_memory_object shared_memory;

    //! Mapped region.
    boost::interprocess::mapped_region mapped_region;

    //! Reader.
    details::no_wait_bytes_queue_reader<> reader;

    /*!
     * \brief Constructor.
     *
     * \param[in] data Data.
     */
    explicit impl_type(details::no_wait_stream_data&& data)
        : shared_memory(std::move(data.shared_memory)),
          mapped_region(std::move(data.mapped_region)),
          reader(*data.atomic_indices, data.buffer) {}
};

no_wait_stream_reader::no_wait_stream_reader() : impl_(nullptr) {}

no_wait_stream_reader::~no_wait_stream_reader() noexcept { close(); }

void no_wait_stream_reader::open(
    string_view name, shm_stream_size_t buffer_size) {
    close();
    impl_ =
        new impl_type(details::prepare_no_wait_stream_data(name, buffer_size));
}

void no_wait_stream_reader::close() noexcept {
    if (impl_ == nullptr) {
        return;
    }
    delete impl_;
    impl_ = nullptr;
}

shm_stream_size_t no_wait_stream_reader::available_size() const noexcept {
    if (impl_ == nullptr) {
        return 0U;
    }
    return impl_->reader.available_size();
}

bytes_view no_wait_stream_reader::try_reserve(
    shm_stream_size_t expected_size) noexcept {
    if (impl_ == nullptr) {
        return bytes_view(nullptr, 0U);
    }
    return impl_->reader.try_reserve(expected_size);
}

bytes_view no_wait_stream_reader::try_reserve() noexcept {
    if (impl_ == nullptr) {
        return bytes_view(nullptr, 0U);
    }
    return impl_->reader.try_reserve();
}

void no_wait_stream_reader::commit(shm_stream_size_t read_size) noexcept {
    if (impl_ == nullptr) {
        return;
    }
    impl_->reader.commit(read_size);
}

}  // namespace shm_stream
