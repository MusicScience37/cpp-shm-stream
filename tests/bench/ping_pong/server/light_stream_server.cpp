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
 * \brief Implementation of light_stream_server class.
 */
#include "light_stream_server.h"

#include <atomic>

#include "../common.h"
#include "shm_stream/common_types.h"

namespace shm_stream_test {

light_stream_server::light_stream_server() {
    input_.open(request_stream_name(), buffer_size());
    output_.open(response_stream_name(), buffer_size());
}

light_stream_server::~light_stream_server() { stop(); }

void light_stream_server::start() {
    is_stopped_.store(false, std::memory_order_relaxed);
    thread_ = std::thread{[this] { this->run(); }};
}

void light_stream_server::stop() {
    is_stopped_.store(true, std::memory_order_relaxed);
    if (thread_.joinable()) {
        thread_.join();
    }
}

void light_stream_server::run() {
    while (true) {
        const auto input_buffer = input_.try_reserve();
        if (input_buffer.empty()) {
            if (is_stopped_.load(std::memory_order_relaxed)) {
                return;
            }
            std::this_thread::yield();
            continue;
        }

        for (auto iter = input_buffer.data(),
                  end = input_buffer.data() + input_buffer.size();
             iter != end;) {
            const auto output_buffer = output_.try_reserve();
            if (output_buffer.empty()) {
                std::this_thread::yield();
                continue;
            }

            const std::ptrdiff_t writable_size =
                std::min<std::ptrdiff_t>(output_buffer.size(), end - iter);
            std::copy(iter, iter + writable_size, output_buffer.data());
            output_.commit(
                static_cast<shm_stream::shm_stream_size_t>(writable_size));

            iter += writable_size;
        }

        input_.commit(input_buffer.size());
    }
}

}  // namespace shm_stream_test
