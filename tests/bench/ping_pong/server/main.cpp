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
 * \brief Definition of main function.
 */
#include <csignal>
#include <cstdio>
#include <exception>
#include <memory>
#include <unordered_map>

#include <fmt/format.h>

#include "command_server.h"
#include "light_stream_server.h"
#include "server_base.h"

extern "C" void on_signal(int /*number*/);

std::atomic<bool> is_stopped{false};

void on_signal(int /*number*/) {
    is_stopped.store(true, std::memory_order_relaxed);
}

int main() {
    try {
        std::signal(SIGINT, on_signal);
        std::signal(SIGTERM, on_signal);

        std::unordered_map<shm_stream_test::protocol_type,
            std::shared_ptr<shm_stream_test::server_base>>
            bench_server{};
        bench_server.emplace(shm_stream_test::protocol_type::light_stream,
            std::make_shared<shm_stream_test::light_stream_server>());

        shm_stream_test::command_server command_server{std::move(bench_server)};

        while (!is_stopped.load(std::memory_order_relaxed)) {
            // NOLINTNEXTLINE
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        return 0;
    } catch (const std::exception& e) {
        fmt::print(stderr, "Exception thrown: {}", e.what());
        return 1;
    }
}
