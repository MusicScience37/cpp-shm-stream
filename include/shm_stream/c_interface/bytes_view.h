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
 * \brief Definition of c_shm_stream_bytes_view struct.
 */
#pragma once

#include "shm_stream/c_interface/common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Struct of views of constant byte sequences.
 */
struct c_shm_stream_bytes_view {
    //! Pointer to data.
    const char* data;

    //! Size of data.
    c_shm_stream_size_t size;
};

/*!
 * \brief Struct of views of constant byte sequences.
 */
typedef struct c_shm_stream_bytes_view c_shm_stream_bytes_view_t;

/*!
 * \brief Struct of views of non-constant byte sequences.
 */
struct c_shm_stream_mutable_bytes_view {
    //! Pointer to data.
    char* data;

    //! Size of data.
    c_shm_stream_size_t size;
};

/*!
 * \brief Struct of views of non-constant byte sequences.
 */
typedef struct c_shm_stream_mutable_bytes_view
    c_shm_stream_mutable_bytes_view_t;

#ifdef __cplusplus
}
#endif
