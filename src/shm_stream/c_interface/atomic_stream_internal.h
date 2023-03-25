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
 * \brief Definition of internal functions of streams based on atomic variables.
 */
#pragma once

#include <string>

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#include "atomic_stream_internal.h"
#include "shm_stream/bytes_view.h"
#include "shm_stream/details/atomic_index_pair.h"

namespace shm_stream {
namespace details {

/*!
 * \brief Data of streams based on atomic variables.
 */
struct atomic_stream_data {
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
 * \brief Initialize data of streams from shared memory.
 *
 * \param[in,out] data Data.
 * \param[in] buffer_size Size of the buffer.
 */
void init_stream_data_from_shared_memory(
    atomic_stream_data& data, shm_stream_size_t buffer_size);

/*!
 * \brief Extract data of streams from shared memory.
 *
 * \param[in,out] data Data.
 */
void extract_stream_data_from_shared_memory(atomic_stream_data& data);

/*!
 * \brief Remove a stream based on atomic variables.
 *
 * \param[in] mutex_name Name of the mutex.
 * \param[in] shm_name Name of the shared memory.
 */
void remove_atomic_stream(
    const std::string& mutex_name, const std::string& shm_name);

}  // namespace details
}  // namespace shm_stream
