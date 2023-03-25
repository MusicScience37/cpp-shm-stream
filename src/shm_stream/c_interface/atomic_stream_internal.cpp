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
 * \brief Implementation of internal functions of streams based on atomic
 * variables.
 */
#include "atomic_stream_internal.h"

#include <mutex>

#include <boost/interprocess/creation_tags.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include "shm_stream/common_types.h"

namespace shm_stream {
namespace details {

/*!
 * \brief Header of the data shared in streams based on atomic variables.
 */
struct atomic_stream_header {
    //! Atomic variables of indices.
    alignas(cache_line_size()) details::atomic_index_pair<> indices{};

    //! Size of the buffer.
    alignas(cache_line_size()) shm_stream_size_t buffer_size{};
};

static_assert(sizeof(atomic_stream_header) == 3U * cache_line_size(),
    "Unexpected size of atomic_stream_header.");

void init_stream_data_from_shared_memory(
    atomic_stream_data& data, shm_stream_size_t buffer_size) {
    const boost::interprocess::offset_t data_size =
        static_cast<boost::interprocess::offset_t>(
            sizeof(atomic_stream_header)) +
        static_cast<boost::interprocess::offset_t>(buffer_size);
    data.shared_memory.truncate(data_size);

    data.mapped_region = boost::interprocess::mapped_region(
        data.shared_memory, boost::interprocess::read_write);

    auto* header =
        new (data.mapped_region.get_address()) atomic_stream_header();
    header->indices.writer() = 0U;
    header->indices.reader() = 0U;
    header->buffer_size = buffer_size;

    data.atomic_indices = &header->indices;
    data.buffer =
        mutable_bytes_view(static_cast<char*>(static_cast<void*>(header + 1)),
            header->buffer_size);
}

void extract_stream_data_from_shared_memory(atomic_stream_data& data) {
    data.mapped_region = boost::interprocess::mapped_region(
        data.shared_memory, boost::interprocess::read_write);

    auto* header =
        static_cast<atomic_stream_header*>(data.mapped_region.get_address());
    data.atomic_indices = &header->indices;
    data.buffer =
        mutable_bytes_view(static_cast<char*>(static_cast<void*>(header + 1)),
            header->buffer_size);
}

void remove_atomic_stream(
    const std::string& mutex_name, const std::string& shm_name) {
    {
        boost::interprocess::named_mutex mutex{
            boost::interprocess::open_or_create, mutex_name.c_str()};
        {
            std::unique_lock<boost::interprocess::named_mutex> lock(mutex);

            boost::interprocess::shared_memory_object::remove(shm_name.c_str());
        }
    }
    boost::interprocess::named_mutex::remove(mutex_name.c_str());
}

}  // namespace details
}  // namespace shm_stream
