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
 * \brief Definition of generate_data function.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <random>
#include <string>

namespace shm_stream_test {

/*!
 * \brief Generate data for tests.
 *
 * \param[in] size Size.
 * \return Data.
 */
[[nodiscard]] inline std::string generate_data(std::size_t size) {
    // NOLINTNEXTLINE(cert-msc32-c, cert-msc51-cpp): for reproducibility of test
    std::mt19937 generator;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    std::uniform_int_distribution<std::uint32_t> dist{0x21, 0x7E};

    std::string data;
    data.reserve(size);
    for (std::size_t i = 0; i < size; ++i) {
        data.push_back(
            static_cast<char>(static_cast<unsigned char>(dist(generator))));
    }
    return data;
}

}  // namespace shm_stream_test
