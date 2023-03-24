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
 * \brief Implementation of internal functions of light streams of bytes without
 * waiting (possibly lock-free and wait-free).
 */
#include "light_stream_internal.h"

#include "shm_stream/c_interface/error_codes.h"
#include "shm_stream/shm_stream_exception.h"

namespace shm_stream {
namespace details {

std::string light_stream_shm_name(string_view stream_name) {
    return fmt::format("shm_stream_light_stream_data_{}", stream_name);
}

std::string light_stream_mutex_name(string_view stream_name) {
    return fmt::format("shm_stream_light_stream_lock_{}", stream_name);
}

light_stream_data create_and_initialize_light_stream_data(
    string_view name, shm_stream_size_t buffer_size) {
    light_stream_data data{};
    const std::string data_shm_name = light_stream_shm_name(name);

    try {
        data.shared_memory = boost::interprocess::shared_memory_object(
            boost::interprocess::create_only, data_shm_name.c_str(),
            boost::interprocess::read_write);
    } catch (...) {
        throw shm_stream_error(c_shm_stream_error_code_failed_to_open);
    }

    const boost::interprocess::offset_t data_size =
        static_cast<boost::interprocess::offset_t>(
            sizeof(light_stream_header)) +
        static_cast<boost::interprocess::offset_t>(buffer_size);
    data.shared_memory.truncate(data_size);

    data.mapped_region = boost::interprocess::mapped_region(
        data.shared_memory, boost::interprocess::read_write);

    auto* header = new (data.mapped_region.get_address()) light_stream_header();
    header->indices.writer() = 0U;
    header->indices.reader() = 0U;
    header->buffer_size = buffer_size;

    data.atomic_indices = &header->indices;
    data.buffer =
        mutable_bytes_view(static_cast<char*>(static_cast<void*>(header + 1)),
            header->buffer_size);

    return data;
}

light_stream_data prepare_light_stream_data(
    string_view name, shm_stream_size_t buffer_size) {
    light_stream_data data{};

    const std::string data_shm_name = light_stream_shm_name(name);
    const std::string mutex_name = light_stream_mutex_name(name);
    boost::interprocess::named_mutex mutex{
        boost::interprocess::open_or_create, mutex_name.c_str()};
    std::unique_lock<boost::interprocess::named_mutex> lock(mutex);

    try {
        data.shared_memory = boost::interprocess::shared_memory_object(
            boost::interprocess::open_only, data_shm_name.c_str(),
            boost::interprocess::read_write);
    } catch (...) {
        // Shared memory doesn't exist, so create one.
        return create_and_initialize_light_stream_data(name, buffer_size);
    }

    data.mapped_region = boost::interprocess::mapped_region(
        data.shared_memory, boost::interprocess::read_write);

    auto* header =
        static_cast<light_stream_header*>(data.mapped_region.get_address());
    data.atomic_indices = &header->indices;
    data.buffer =
        mutable_bytes_view(static_cast<char*>(static_cast<void*>(header + 1)),
            header->buffer_size);

    return data;
}

void remove_light_stream(string_view name) {
    const std::string mutex_name = details::light_stream_mutex_name(name);
    {
        boost::interprocess::named_mutex mutex{
            boost::interprocess::open_or_create, mutex_name.c_str()};
        {
            std::unique_lock<boost::interprocess::named_mutex> lock(mutex);

            boost::interprocess::shared_memory_object::remove(
                details::light_stream_shm_name(name).c_str());
        }
    }
    boost::interprocess::named_mutex::remove(mutex_name.c_str());
}

}  // namespace details
}  // namespace shm_stream
