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
 * \brief Definition of C interface of blocking streams of bytes with wait
 * operations.
 */
#pragma once

#include "shm_stream/c_interface/common_types.h"
#include "shm_stream/c_interface/error_codes.h"
#include "shm_stream/c_interface/string_view.h"
#include "shm_stream/details/shm_stream_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Create a blocking stream of bytes with wait operation.
 *
 * \param[in] name Name of the stream.
 * \param[in] buffer_size Size of the buffer.
 * \return Error code.
 */
SHM_STREAM_EXPORT c_shm_stream_error_code_t c_shm_stream_blocking_stream_create(
    c_shm_stream_string_view_t name, c_shm_stream_size_t buffer_size);

/*!
 * \brief Remove a blocking stream of bytes with wait operation.
 *
 * \param[in] name Name of the stream.
 */
SHM_STREAM_EXPORT void c_shm_stream_blocking_stream_remove(
    c_shm_stream_string_view_t name);

#ifdef __cplusplus
}
#endif
