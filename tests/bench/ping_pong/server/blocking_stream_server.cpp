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
 * \brief Implementation of blocking_stream_server class.
 */
#include "blocking_stream_server.h"

#include <atomic>

#include "../common.h"
#include "shm_stream/blocking_stream.h"
#include "shm_stream/common_types.h"

namespace shm_stream_test {

blocking_stream_server::blocking_stream_server() {
    shm_stream::blocking_stream::remove(request_stream_name());
    shm_stream::blocking_stream::remove(response_stream_name());
    input_.open(request_stream_name(), buffer_size());
    output_.open(response_stream_name(), buffer_size());

    thread_ = std::thread{[this] { this->run(); }};
}

blocking_stream_server::~blocking_stream_server() {
    output_.stop();
    input_.stop();
    if (thread_.joinable()) {
        thread_.join();
    }
}

void blocking_stream_server::start() {
    // No operation.
}

void blocking_stream_server::stop() {
    // No operation.
}

void blocking_stream_server::run() {
    while (true) {
        const auto input_buffer = input_.wait_reserve();
        if (input_buffer.empty()) {
            return;
        }

        for (auto iter = input_buffer.data(),
                  end = input_buffer.data() + input_buffer.size();
             iter != end;) {
            const auto output_buffer = output_.wait_reserve();

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
