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
 * \brief Definition of internal functions of light streams of bytes without
 * waiting (possibly lock-free and wait-free).
 */
#pragma once

#include <mutex>
#include <string>

#include <boost/interprocess/creation_tags.hpp>
#include <boost/interprocess/detail/os_file_functions.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include "shm_stream/bytes_view.h"
#include "shm_stream/common_types.h"
#include "shm_stream/details/atomic_index_pair.h"
#include "shm_stream/details/cache_line_size.h"
#include "shm_stream/string_view.h"

namespace shm_stream {
namespace details {

/*!
 * \brief Header of the data shared in light streams.
 */
struct light_stream_header {
    //! Atomic variables of indices.
    alignas(cache_line_size()) details::atomic_index_pair<> indices{};

    //! Size of the buffer.
    alignas(cache_line_size()) shm_stream_size_t buffer_size{};
};

static_assert(sizeof(light_stream_header) == 3U * cache_line_size(),
    "Unexpected size of light_stream_header.");

/*!
 * \brief Data of light streams.
 */
struct light_stream_data {
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
 * \brief Get the name of the shared memory of a light stream.
 *
 * \param[in] stream_name Name of the stream.
 * \return Name of the shared memory.
 */
[[nodiscard]] std::string light_stream_shm_name(string_view stream_name);

/*!
 * \brief Get the name of the mutex of a light stream.
 *
 * \param[in] stream_name Name of the stream.
 * \return Name of the mutex.
 */
[[nodiscard]] std::string light_stream_mutex_name(string_view stream_name);

/*!
 * \brief Create and initialize data of a light stream.
 *
 * \param[in] name Name of the stream.
 * \param[in] buffer_size Size of the buffer.
 * \return Data.
 */
[[nodiscard]] light_stream_data create_and_initialize_light_stream_data(
    string_view name, shm_stream_size_t buffer_size);

/*!
 * \brief Prepare data of a light stream.
 *
 * \param[in] name Name of the stream.
 * \param[in] buffer_size Size of the buffer.
 * \return Data.
 */
[[nodiscard]] light_stream_data prepare_light_stream_data(
    string_view name, shm_stream_size_t buffer_size);

/*!
 * \brief Remove a light stream.
 *
 * \param[in] name Name of the stream.
 */
void remove_light_stream(string_view name);

}  // namespace details
}  // namespace shm_stream
