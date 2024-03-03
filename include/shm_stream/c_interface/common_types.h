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
 * \brief Definition of common types.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Type of sizes used in this library.
 *
 * \note For compatibility of data shared between processes, I defined this type
 * with a fixed size.
 */
typedef uint32_t c_shm_stream_size_t;

#ifdef __cplusplus
}
#endif
