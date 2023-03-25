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
 * \brief Implementation of reader of streams of bytes without waiting.
 */
#include <cstdio>
#include <exception>
#include <thread>

#include <fmt/format.h>

#include "common_def.h"
#include "shm_stream/blocking_stream.h"

int main() {
    try {
        shm_stream::blocking_stream::remove(stream_name);

        shm_stream::blocking_stream_reader reader;
        reader.open(stream_name, buffer_size);

        while (true) {
            const auto buffer = reader.try_reserve();
            if (buffer.empty() && reader.is_stopped()) {
                return 0;
            }
            std::fwrite(buffer.data(), 1, buffer.size(), stdout);
            reader.commit(buffer.size());
        }
    } catch (const std::exception& e) {
        fmt::print(stderr, "Exception thrown: {}", e.what());
        return 1;
    }
}
