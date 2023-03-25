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
 * \brief Implementation of reader of streams of bytes with wait operations.
 */
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <exception>
#include <thread>

#include <fmt/format.h>

#include "common_def.h"
#include "shm_stream/blocking_stream.h"

extern "C" void on_signal(int /*number*/);

std::atomic<bool> is_stopped{false};

void on_signal(int /*number*/) {
    is_stopped.store(true, std::memory_order_relaxed);
}

int main() {
    try {
        std::signal(SIGINT, on_signal);
        std::signal(SIGTERM, on_signal);

        shm_stream::blocking_stream::remove(stream_name);

        shm_stream::blocking_stream_reader reader;
        reader.open(stream_name, buffer_size);

        std::thread thread{[&reader] {
            while (true) {
                const auto buffer = reader.try_reserve();
                if (buffer.empty() && reader.is_stopped()) {
                    std::fflush(stdout);
                    return;
                }
                std::fwrite(buffer.data(), 1, buffer.size(), stdout);
                reader.commit(buffer.size());
            }
        }};

        while (!is_stopped.load(std::memory_order_relaxed)) {
            // NOLINTNEXTLINE
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        // NOLINTNEXTLINE
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        reader.stop();
        thread.join();
    } catch (const std::exception& e) {
        fmt::print(stderr, "Exception thrown: {}", e.what());
        return 1;
    }
}
