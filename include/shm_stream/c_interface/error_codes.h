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
 * \brief Definition of error codes in this library.
 */
#pragma once

#include "shm_stream/details/shm_stream_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Enumeration of error codes.
 */
enum c_shm_stream_error_code {
    //! No error.
    c_shm_stream_error_code_success = 0,

    //! Invalid argument.
    c_shm_stream_error_code_invalid_argument,

    //! Failed to create or open a stream.
    c_shm_stream_error_code_failed_to_open,

    //! Internal error.
    c_shm_stream_error_code_internal_error
};

/*!
 * \brief Enumeration of error codes.
 */
typedef enum c_shm_stream_error_code c_shm_stream_error_code_t;

/*!
 * \brief Get the message of an error code.
 *
 * \param[in] code Error code.
 * \return Message.
 */
SHM_STREAM_EXPORT const char* c_shm_stream_error_message(
    c_shm_stream_error_code_t code);

#ifdef __cplusplus
}
#endif
