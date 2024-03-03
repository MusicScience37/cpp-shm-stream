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
 * \brief Implementation of functions for error codes in this library.
 */
#include "shm_stream/c_interface/error_codes.h"

const char* c_shm_stream_error_message(c_shm_stream_error_code_t code) {
    switch (code) {
    case c_shm_stream_error_code_success:
        return "Success.";
    case c_shm_stream_error_code_invalid_argument:
        return "Invalid argument.";
    case c_shm_stream_error_code_failed_to_open:
        return "Failed to create or open a stream.";
    case c_shm_stream_error_code_internal_error:
        return "Internal error.";
    }
    return "Invalid error code.";
}
