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
 * \brief Test of blocking streams of bytes with wait operations.
 */
#include "shm_stream/blocking_stream.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <future>
#include <thread>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <catch2/catch_test_macros.hpp>

#include "shm_stream/common_types.h"
#include "shm_stream_test/generate_data.h"

TEST_CASE(
    "blocking_stream_writer, blocking_stream_reader (non-blocking APIs)") {
    using shm_stream::blocking_stream_reader;
    using shm_stream::blocking_stream_writer;
    using shm_stream::shm_stream_size_t;
    using shm_stream_test::generate_data;

    const std::string stream_name = "blocking_stream_test";
    shm_stream::blocking_stream::remove(stream_name);

    SECTION("send data") {
        blocking_stream_writer writer;
        blocking_stream_reader reader;

        constexpr shm_stream_size_t buffer_size = 10U;

        constexpr shm_stream_size_t data_size = 100U;
        const std::string data = generate_data(data_size);

        std::promise<void> written_promise;
        auto written_future = written_promise.get_future();
        std::thread writer_thread{[&writer, &stream_name, &data,
                                      &written_promise] {
            writer.open(stream_name, buffer_size);

            auto data_iter = data.cbegin();
            const auto data_end = data.cend();
            while (true) {
                const auto buffer = writer.try_reserve();
                if (buffer.empty()) {
                    std::this_thread::yield();
                    continue;
                }

                const std::ptrdiff_t writable_size = std::min<std::ptrdiff_t>(
                    buffer.size(), data_end - data_iter);
                std::copy(data_iter, data_iter + writable_size, buffer.data());
                writer.commit(static_cast<shm_stream_size_t>(writable_size));
                data_iter += writable_size;

                if (data_iter == data_end) {
                    written_promise.set_value_at_thread_exit();
                    return;
                }
            }
        }};

        std::promise<void> read_promise;
        auto read_future = read_promise.get_future();
        std::string read_data;
        read_data.resize(data_size, ' ');
        std::thread reader_thread{[&reader, &stream_name, &read_data,
                                      &read_promise] {
            reader.open(stream_name, buffer_size);

            auto data_iter = read_data.begin();
            const auto data_end = read_data.end();
            while (true) {
                const auto buffer = reader.try_reserve();
                if (buffer.empty()) {
                    std::this_thread::yield();
                    continue;
                }

                const std::ptrdiff_t readable_size = std::min<std::ptrdiff_t>(
                    buffer.size(), data_end - data_iter);
                std::copy(
                    buffer.data(), buffer.data() + readable_size, data_iter);
                reader.commit(static_cast<shm_stream_size_t>(readable_size));
                data_iter += readable_size;

                if (data_iter == data_end) {
                    read_promise.set_value_at_thread_exit();
                    return;
                }
            }
        }};

        constexpr auto timeout = std::chrono::seconds(10);
        REQUIRE(written_future.wait_for(timeout) == std::future_status::ready);
        REQUIRE(read_future.wait_for(timeout) == std::future_status::ready);

        writer_thread.join();
        reader_thread.join();

        CHECK(read_data == data);
        CHECK(reader.available_size() == 0U);
    }

    shm_stream::blocking_stream::remove(stream_name);
}

TEST_CASE("blocking_stream_writer, blocking_stream_reader (blocking APIs)") {
    using shm_stream::blocking_stream_reader;
    using shm_stream::blocking_stream_writer;
    using shm_stream::shm_stream_size_t;
    using shm_stream_test::generate_data;

    const std::string stream_name = "blocking_stream_test";
    shm_stream::blocking_stream::remove(stream_name);

    SECTION("send data") {
        blocking_stream_writer writer;
        blocking_stream_reader reader;

        constexpr shm_stream_size_t buffer_size = 10U;

        constexpr shm_stream_size_t data_size = 100U;
        const std::string data = generate_data(data_size);

        std::promise<void> written_promise;
        auto written_future = written_promise.get_future();
        std::thread writer_thread{[&writer, &stream_name, &data,
                                      &written_promise] {
            writer.open(stream_name, buffer_size);

            auto data_iter = data.cbegin();
            const auto data_end = data.cend();
            while (true) {
                const auto buffer = writer.wait_reserve();

                const std::ptrdiff_t writable_size = std::min<std::ptrdiff_t>(
                    buffer.size(), data_end - data_iter);
                std::copy(data_iter, data_iter + writable_size, buffer.data());
                writer.commit(static_cast<shm_stream_size_t>(writable_size));
                data_iter += writable_size;

                if (data_iter == data_end) {
                    written_promise.set_value_at_thread_exit();
                    return;
                }
            }
        }};

        std::promise<void> read_promise;
        auto read_future = read_promise.get_future();
        std::string read_data;
        read_data.resize(data_size, ' ');
        std::thread reader_thread{[&reader, &stream_name, &read_data,
                                      &read_promise] {
            reader.open(stream_name, buffer_size);

            auto data_iter = read_data.begin();
            const auto data_end = read_data.end();
            while (true) {
                const auto buffer = reader.wait_reserve();

                const std::ptrdiff_t readable_size = std::min<std::ptrdiff_t>(
                    buffer.size(), data_end - data_iter);
                std::copy(
                    buffer.data(), buffer.data() + readable_size, data_iter);
                reader.commit(static_cast<shm_stream_size_t>(readable_size));
                data_iter += readable_size;

                if (data_iter == data_end) {
                    read_promise.set_value_at_thread_exit();
                    return;
                }
            }
        }};

        constexpr auto timeout = std::chrono::seconds(10);
        REQUIRE(written_future.wait_for(timeout) == std::future_status::ready);
        REQUIRE(read_future.wait_for(timeout) == std::future_status::ready);

        writer_thread.join();
        reader_thread.join();

        CHECK(read_data == data);
        CHECK(reader.available_size() == 0U);
    }

    shm_stream::blocking_stream::remove(stream_name);
}
