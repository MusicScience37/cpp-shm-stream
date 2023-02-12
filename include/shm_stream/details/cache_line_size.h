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
 * \brief Definition of atomic_index_pair class.
 */
#pragma once

#include <cstddef>

#include "shm_stream/common_types.h"

namespace shm_stream {
namespace details {

/*!
 * \brief Get the size of cache lines.
 *
 * \return Size of cache line.
 *
 * \note This function returns a fixed value in all environment because some
 * compilers didn't implement a way to get the value using C++ standard library.
 */
[[nodiscard]] inline constexpr std::size_t cache_line_size() {
    return 64U;  // NOLINT
}

}  // namespace details
}  // namespace shm_stream
