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
 * \brief Definition of throw_if_error function.
 */
#pragma once

#include "shm_stream/c_interface/error_codes.h"
#include "shm_stream/shm_stream_exception.h"

namespace shm_stream {
namespace details {

/*!
 * \brief Check an error code and throw an exception if an error.
 *
 * \param[in] code Error code.
 */
inline void throw_if_error(c_shm_stream_error_code_t code) {
    if (code == c_shm_stream_error_code_success) {
        return;
    }
    throw shm_stream_error(code);
}

}  // namespace details
}  // namespace shm_stream
