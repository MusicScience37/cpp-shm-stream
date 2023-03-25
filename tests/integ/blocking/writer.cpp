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
 * \brief Implementation of writer of streams of bytes with wait operations.
 */
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <iostream>
#include <string>
#include <thread>

#include <fmt/format.h>

#include "common_def.h"
#include "shm_stream/blocking_stream.h"
#include "shm_stream/common_types.h"

int main() {
    try {
        shm_stream::blocking_stream_writer writer;
        writer.open(stream_name, buffer_size);

        std::string line;
        while (true) {
            line.clear();
            std::getline(std::cin, line);
            if (line.empty()) {
                return 0;
            }
            line += '\n';

            for (auto iter = line.cbegin(), end = line.cend(); iter != end;) {
                const auto buffer = writer.wait_reserve();
                const auto writable_size =
                    std::min<std::ptrdiff_t>(buffer.size(), end - iter);
                std::copy(iter, iter + writable_size, buffer.data());
                writer.commit(
                    static_cast<shm_stream::shm_stream_size_t>(writable_size));
                iter += writable_size;
            }
        }
    } catch (const std::exception& e) {
        fmt::print(stderr, "Exception thrown: {}", e.what());
        return 1;
    }
}
