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
 * \brief Implementation of internal functions of blocking streams of bytes with
 * wait operations.
 */
#include "blocking_stream_internal.h"

#include <boost/interprocess/creation_tags.hpp>
#include <boost/interprocess/detail/os_file_functions.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <fmt/format.h>

#include "atomic_stream_internal.h"
#include "shm_stream/c_interface/error_codes.h"
#include "shm_stream/shm_stream_exception.h"

namespace shm_stream {
namespace details {

std::string blocking_stream_shm_name(string_view stream_name) {
    return fmt::format("shm_stream_blocking_stream_data_{}", stream_name);
}

std::string blocking_stream_mutex_name(string_view stream_name) {
    return fmt::format("shm_stream_blocking_stream_lock_{}", stream_name);
}

blocking_stream_data create_and_initialize_blocking_stream_data(
    string_view name, shm_stream_size_t buffer_size) {
    blocking_stream_data data{};
    const std::string data_shm_name = blocking_stream_shm_name(name);

    try {
        data.shared_memory = boost::interprocess::shared_memory_object(
            boost::interprocess::create_only, data_shm_name.c_str(),
            boost::interprocess::read_write);
    } catch (...) {
        throw shm_stream_error(c_shm_stream_error_code_failed_to_open);
    }

    init_stream_data_from_shared_memory(data, buffer_size);

    return data;
}

blocking_stream_data prepare_blocking_stream_data(
    string_view name, shm_stream_size_t buffer_size) {
    blocking_stream_data data{};

    const std::string data_shm_name = blocking_stream_shm_name(name);
    const std::string mutex_name = blocking_stream_mutex_name(name);
    boost::interprocess::named_mutex mutex{
        boost::interprocess::open_or_create, mutex_name.c_str()};
    std::unique_lock<boost::interprocess::named_mutex> lock(mutex);

    try {
        data.shared_memory = boost::interprocess::shared_memory_object(
            boost::interprocess::open_only, data_shm_name.c_str(),
            boost::interprocess::read_write);
    } catch (...) {
        // Shared memory doesn't exist, so create one.
        return create_and_initialize_blocking_stream_data(name, buffer_size);
    }

    extract_stream_data_from_shared_memory(data);

    return data;
}

void remove_blocking_stream(string_view name) {
    const std::string mutex_name = details::blocking_stream_mutex_name(name);
    const std::string shm_name = details::blocking_stream_shm_name(name);
    remove_atomic_stream(mutex_name, shm_name);
}

}  // namespace details
}  // namespace shm_stream
