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
 * \brief Implementation of C interface of blocking streams of bytes with wait
 * operations.
 */
#include "shm_stream/c_interface/blocking_stream_reader.h"

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#include "blocking_stream_internal.h"
#include "shm_stream/c_interface/bytes_view.h"
#include "shm_stream/c_interface/translate_error.h"
#include "shm_stream/common_types.h"
#include "shm_stream/details/blocking_bytes_queue.h"
#include "shm_stream/string_view.h"

/*!
 * \brief Reader of blocking streams of bytes with wait operations.
 */
struct c_shm_stream_blocking_stream_reader {
    //! Shared memory object.
    boost::interprocess::shared_memory_object shared_memory;

    //! Mapped region.
    boost::interprocess::mapped_region mapped_region;

    //! Reader.
    shm_stream::details::blocking_bytes_queue_reader<> reader;

    /*!
     * \brief Constructor.
     *
     * \param[in] data Data.
     */
    explicit c_shm_stream_blocking_stream_reader(
        shm_stream::details::blocking_stream_data&& data)
        : shared_memory(std::move(data.shared_memory)),
          mapped_region(std::move(data.mapped_region)),
          reader(*data.atomic_indices, data.buffer) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] name Name of the stream.
     * \param[in] buffer_size Size of the buffer.
     */
    c_shm_stream_blocking_stream_reader(
        shm_stream::string_view name, shm_stream::shm_stream_size_t buffer_size)
        : c_shm_stream_blocking_stream_reader(
              shm_stream::details::prepare_blocking_stream_data(
                  name, buffer_size)) {}
};

c_shm_stream_error_code_t c_shm_stream_blocking_stream_reader_create(
    c_shm_stream_blocking_stream_reader_t** reader,
    c_shm_stream_string_view_t name, c_shm_stream_size_t buffer_size) {
    C_SHM_STREAM_TRANSLATE_ERROR(*reader =
                                     new c_shm_stream_blocking_stream_reader(
                                         shm_stream::string_view{
                                             name.data, name.size},
                                         buffer_size));
}

void c_shm_stream_blocking_stream_reader_destroy(
    c_shm_stream_blocking_stream_reader_t* reader) {
    delete reader;
}

c_shm_stream_size_t c_shm_stream_blocking_stream_reader_available_size(
    c_shm_stream_blocking_stream_reader_t* reader) {
    if (reader == nullptr) {
        return 0U;
    }
    return reader->reader.available_size();
}

c_shm_stream_bytes_view_t c_shm_stream_blocking_stream_reader_try_reserve(
    c_shm_stream_blocking_stream_reader_t* reader,
    c_shm_stream_size_t expected_size) {
    if (reader == nullptr) {
        return c_shm_stream_bytes_view_t{nullptr, 0U};
    }
    const auto buf = reader->reader.try_reserve(expected_size);
    return c_shm_stream_bytes_view_t{buf.data(), buf.size()};
}

c_shm_stream_bytes_view_t c_shm_stream_blocking_stream_reader_try_reserve_all(
    c_shm_stream_blocking_stream_reader_t* reader) {
    if (reader == nullptr) {
        return c_shm_stream_bytes_view_t{nullptr, 0U};
    }
    const auto buf = reader->reader.try_reserve();
    return c_shm_stream_bytes_view_t{buf.data(), buf.size()};
}

void c_shm_stream_blocking_stream_reader_commit(
    c_shm_stream_blocking_stream_reader_t* reader,
    c_shm_stream_size_t read_size) {
    if (reader == nullptr) {
        return;
    }
    reader->reader.commit(read_size);
}
