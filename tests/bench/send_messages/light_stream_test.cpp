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
 * \brief Benchmark of streams of bytes without waiting.
 */
#include "shm_stream/light_stream.h"

#include <atomic>
#include <thread>

#include <stat_bench/benchmark_macros.h>

#include "send_messages_fixture.h"
#include "shm_stream/bytes_view.h"

STAT_BENCH_CASE_F(
    shm_stream_test::send_messages_fixture, "send_messages", "light_stream") {
    using shm_stream::light_stream_reader;
    using shm_stream::light_stream_writer;
    using shm_stream::shm_stream_size_t;

    const std::string& data = this->get_data();
    const std::size_t data_size = data.size();
    const std::size_t buffer_size = 10 * data_size;

    const std::string stream_name = "light_stream_test";
    shm_stream::light_stream::remove(stream_name);

    light_stream_writer writer;
    writer.open(stream_name, buffer_size);

    light_stream_reader reader;
    reader.open(stream_name, buffer_size);

    std::atomic<bool> is_running{true};
    std::thread reader_thread{[&reader, &is_running] {
        while (true) {
            const auto buffer = reader.try_reserve();
            if (buffer.empty()) {
                if (!is_running.load(std::memory_order_relaxed)) {
                    return;
                }
                std::this_thread::yield();
                continue;
            }
            reader.commit(buffer.size());
        }
    }};

    STAT_BENCH_MEASURE() {
        for (auto data_iter = data.cbegin(), data_end = data.cend();
             data_iter != data_end;) {
            const auto buffer = writer.try_reserve();
            const std::ptrdiff_t writable_size =
                std::min<std::ptrdiff_t>(buffer.size(), data_end - data_iter);
            std::copy(data_iter, data_iter + writable_size, buffer.data());
            writer.commit(static_cast<shm_stream_size_t>(writable_size));
            data_iter += writable_size;

            if (data_iter == data_end) {
                break;
            }
        }
    };

    is_running.store(false, std::memory_order_relaxed);
    reader_thread.join();
}
