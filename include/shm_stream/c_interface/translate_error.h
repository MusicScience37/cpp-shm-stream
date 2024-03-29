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
 * \brief Definition of C_SHM_STREAM_TRANSLATE_ERROR macro.
 */
#pragma once

#include "shm_stream/c_interface/error_codes.h"
#include "shm_stream/shm_stream_exception.h"

/*!
 * \brief Translate errors in C functions.
 */
#define C_SHM_STREAM_TRANSLATE_ERROR(EXPRESSION)            \
    /* NOLINTNEXTLINE(cppcoreguidelines-avoid-do-while) */  \
    do {                                                    \
        try {                                               \
            EXPRESSION;                                     \
            return c_shm_stream_error_code_success;         \
        } catch (const ::shm_stream::shm_stream_error& e) { \
            return e.code();                                \
        } catch (...) {                                     \
            return c_shm_stream_error_code_internal_error;  \
        }                                                   \
    } while (false)

/*!
 * \brief Ignore errors in C functions.
 */
#define C_SHM_STREAM_NO_ERROR(EXPRESSION)                  \
    /* NOLINTNEXTLINE(cppcoreguidelines-avoid-do-while) */ \
    do {                                                   \
        try {                                              \
            EXPRESSION;                                    \
        } catch (...) {                                    \
            /* No operation for errors. */                 \
        }                                                  \
    } while (false)
