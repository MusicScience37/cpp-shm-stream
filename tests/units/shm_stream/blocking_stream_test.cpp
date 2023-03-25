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

#include <chrono>
#include <future>
#include <thread>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <catch2/catch_test_macros.hpp>

#include "shm_stream/bytes_view.h"
#include "shm_stream/common_types.h"

TEST_CASE("shm_stream::blocking_stream_writer") {
    using shm_stream::blocking_stream_writer;
    using shm_stream::shm_stream_size_t;

    const std::string stream_name = "blocking_stream_writer_test";
    boost::interprocess::shared_memory_object::remove(
        ("shm_stream_blocking_stream_data_" + stream_name).c_str());
    boost::interprocess::named_mutex::remove(
        ("shm_stream_blocking_stream_lock_" + stream_name).c_str());

    SECTION("open a stream") {
        blocking_stream_writer writer;
        CHECK_FALSE(writer.is_opened());

        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.is_opened());
    }

    SECTION("move construct") {
        blocking_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.is_opened());

        blocking_stream_writer moved{std::move(writer)};
        CHECK(moved.is_opened());
        CHECK_FALSE(writer.is_opened());  // NOLINT
    }

    SECTION("close a stream explicitly") {
        blocking_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.is_opened());

        writer.close();

        CHECK_FALSE(writer.is_opened());
    }

    SECTION("get the available size") {
        blocking_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);

        CHECK(writer.available_size() == buffer_size - 1U);
    }

    SECTION("wait available bytes") {
        blocking_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);

        CHECK(writer.wait() == buffer_size - 1U);
    }

    SECTION("stop stream") {
        blocking_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.available_size() == buffer_size - 1U);
        CHECK_FALSE(writer.is_stopped());

        writer.stop();

        CHECK(writer.available_size() == 0U);
        CHECK(writer.is_stopped());
    }

    SECTION("reserve with size") {
        blocking_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.available_size() == buffer_size - 1U);

        constexpr shm_stream_size_t expected_size = 5U;
        const auto buffer = writer.try_reserve(expected_size);

        CHECK(buffer.size() == expected_size);
    }

    SECTION("reserve with large size") {
        blocking_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.available_size() == buffer_size - 1U);

        constexpr shm_stream_size_t expected_size = 100U;
        const auto buffer = writer.try_reserve(expected_size);

        CHECK(buffer.size() == buffer_size - 1U);
    }

    SECTION("reserve bytes as many as possible") {
        blocking_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.available_size() == buffer_size - 1U);

        const auto buffer = writer.try_reserve();

        CHECK(buffer.size() == buffer_size - 1U);
    }

    SECTION("reserve with size with wait") {
        blocking_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.available_size() == buffer_size - 1U);

        constexpr shm_stream_size_t expected_size = 5U;
        const auto buffer = writer.wait_reserve(expected_size);

        CHECK(buffer.size() == expected_size);
    }

    SECTION("reserve with large size with wait") {
        blocking_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.available_size() == buffer_size - 1U);

        constexpr shm_stream_size_t expected_size = 100U;
        const auto buffer = writer.wait_reserve(expected_size);

        CHECK(buffer.size() == buffer_size - 1U);
    }

    SECTION("reserve bytes as many as possible with wait") {
        blocking_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.available_size() == buffer_size - 1U);

        const auto buffer = writer.wait_reserve();

        CHECK(buffer.size() == buffer_size - 1U);
    }

    SECTION("commit written bytes") {
        blocking_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.available_size() == buffer_size - 1U);
        (void)writer.wait_reserve();

        writer.commit(3U);

        CHECK(writer.available_size() == buffer_size - 4U);  // NOLINT
    }

    boost::interprocess::shared_memory_object::remove(
        ("shm_stream_blocking_stream_data_" + stream_name).c_str());
    boost::interprocess::named_mutex::remove(
        ("shm_stream_blocking_stream_lock_" + stream_name).c_str());
}

TEST_CASE("shm_stream::blocking_stream_reader") {
    using shm_stream::blocking_stream_reader;
    using shm_stream::blocking_stream_writer;
    using shm_stream::bytes_view;
    using shm_stream::shm_stream_size_t;

    const std::string stream_name = "blocking_stream_reader_test";
    boost::interprocess::shared_memory_object::remove(
        ("shm_stream_blocking_stream_data_" + stream_name).c_str());
    boost::interprocess::named_mutex::remove(
        ("shm_stream_blocking_stream_lock_" + stream_name).c_str());

    constexpr auto timeout = std::chrono::seconds(1);

    SECTION("open a stream") {
        blocking_stream_reader reader;
        CHECK_FALSE(reader.is_opened());

        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);
        CHECK(reader.is_opened());
    }

    SECTION("move construct") {
        blocking_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);
        CHECK(reader.is_opened());

        blocking_stream_reader moved{std::move(reader)};
        CHECK(moved.is_opened());
        CHECK_FALSE(reader.is_opened());  // NOLINT
    }

    SECTION("close a stream explicitly") {
        blocking_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);
        CHECK(reader.is_opened());

        reader.close();

        CHECK_FALSE(reader.is_opened());
    }

    SECTION("get the available size") {
        blocking_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);
        blocking_stream_writer writer;
        writer.open(stream_name, buffer_size);
        (void)writer.try_reserve();
        writer.commit(3U);

        CHECK(reader.available_size() == 3U);
    }

    SECTION("wait available bytes") {
        blocking_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);

        std::promise<shm_stream_size_t> promise;
        auto future = promise.get_future();
        std::thread thread{[&reader, &promise] {
            const auto available = reader.wait();
            promise.set_value_at_thread_exit(available);
        }};

        blocking_stream_writer writer;
        writer.open(stream_name, buffer_size);
        (void)writer.try_reserve();
        writer.commit(3U);

        REQUIRE(future.wait_for(timeout) == std::future_status::ready);
        thread.join();

        CHECK(future.get() == 3U);
    }

    SECTION("stop stream") {
        blocking_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);
        CHECK_FALSE(reader.is_stopped());

        reader.stop();

        CHECK(reader.is_stopped());
    }

    SECTION("reserve with size") {
        blocking_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);
        blocking_stream_writer writer;
        writer.open(stream_name, buffer_size);
        (void)writer.try_reserve();
        writer.commit(3U);

        constexpr shm_stream_size_t expected_size = 2U;
        const auto buffer = reader.try_reserve(expected_size);

        CHECK(buffer.size() == expected_size);
    }

    SECTION("reserve with large size") {
        blocking_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);
        blocking_stream_writer writer;
        writer.open(stream_name, buffer_size);
        (void)writer.try_reserve();
        writer.commit(3U);

        constexpr shm_stream_size_t expected_size = 100U;
        const auto buffer = reader.try_reserve(expected_size);

        CHECK(buffer.size() == 3U);
    }

    SECTION("reserve bytes as many as possible") {
        blocking_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);
        blocking_stream_writer writer;
        writer.open(stream_name, buffer_size);
        (void)writer.try_reserve();
        writer.commit(3U);

        const auto buffer = reader.try_reserve();

        CHECK(buffer.size() == 3U);
    }

    SECTION("reserve with size with wait") {
        blocking_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);

        static constexpr shm_stream_size_t expected_size = 2U;
        std::promise<bytes_view> promise;
        auto future = promise.get_future();
        std::thread thread{[&reader, &promise] {
            const auto buffer = reader.wait_reserve(expected_size);
            promise.set_value_at_thread_exit(buffer);
        }};

        blocking_stream_writer writer;
        writer.open(stream_name, buffer_size);
        (void)writer.try_reserve();
        writer.commit(3U);

        REQUIRE(future.wait_for(timeout) == std::future_status::ready);
        thread.join();

        const auto buffer = future.get();
        CHECK(buffer.size() == expected_size);
    }

    SECTION("reserve with large size with wait") {
        blocking_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);

        static constexpr shm_stream_size_t expected_size = 100U;
        std::promise<bytes_view> promise;
        auto future = promise.get_future();
        std::thread thread{[&reader, &promise] {
            const auto buffer = reader.wait_reserve(expected_size);
            promise.set_value_at_thread_exit(buffer);
        }};

        blocking_stream_writer writer;
        writer.open(stream_name, buffer_size);
        (void)writer.try_reserve();
        writer.commit(3U);

        REQUIRE(future.wait_for(timeout) == std::future_status::ready);
        thread.join();

        const auto buffer = future.get();
        CHECK(buffer.size() == 3U);
    }

    SECTION("reserve bytes as many as possible with wait") {
        blocking_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);

        std::promise<bytes_view> promise;
        auto future = promise.get_future();
        std::thread thread{[&reader, &promise] {
            const auto buffer = reader.wait_reserve();
            promise.set_value_at_thread_exit(buffer);
        }};

        blocking_stream_writer writer;
        writer.open(stream_name, buffer_size);
        (void)writer.try_reserve();
        writer.commit(3U);

        REQUIRE(future.wait_for(timeout) == std::future_status::ready);
        thread.join();

        const auto buffer = future.get();
        CHECK(buffer.size() == 3U);
    }

    SECTION("commit read bytes") {
        blocking_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);
        blocking_stream_writer writer;
        writer.open(stream_name, buffer_size);
        (void)writer.try_reserve();
        writer.commit(3U);
        (void)reader.try_reserve();

        reader.commit(2U);

        CHECK(reader.available_size() == 1U);
    }

    boost::interprocess::shared_memory_object::remove(
        ("shm_stream_blocking_stream_data_" + stream_name).c_str());
    boost::interprocess::named_mutex::remove(
        ("shm_stream_blocking_stream_lock_" + stream_name).c_str());
}

TEST_CASE("shm_stream::blocking_stream") {
    using shm_stream::shm_stream_size_t;

    const std::string stream_name = "blocking_stream_reader_test";

    boost::interprocess::shared_memory_object::remove(
        ("shm_stream_blocking_stream_data_" + stream_name).c_str());
    boost::interprocess::named_mutex::remove(
        ("shm_stream_blocking_stream_lock_" + stream_name).c_str());

    SECTION("create a stream") {
        constexpr shm_stream_size_t buffer_size = 10U;
        shm_stream::blocking_stream::create(stream_name, buffer_size);

        CHECK(boost::interprocess::shared_memory_object::remove(
            ("shm_stream_blocking_stream_data_" + stream_name).c_str()));
        CHECK(boost::interprocess::named_mutex::remove(
            ("shm_stream_blocking_stream_lock_" + stream_name).c_str()));
    }

    SECTION("remove a stream") {
        constexpr shm_stream_size_t buffer_size = 10U;
        shm_stream::blocking_stream::create(stream_name, buffer_size);
        shm_stream::blocking_stream::remove(stream_name);

        CHECK_FALSE(boost::interprocess::shared_memory_object::remove(
            ("shm_stream_blocking_stream_data_" + stream_name).c_str()));
        CHECK_FALSE(boost::interprocess::named_mutex::remove(
            ("shm_stream_blocking_stream_lock_" + stream_name).c_str()));
    }

    boost::interprocess::shared_memory_object::remove(
        ("shm_stream_blocking_stream_data_" + stream_name).c_str());
    boost::interprocess::named_mutex::remove(
        ("shm_stream_blocking_stream_lock_" + stream_name).c_str());
}
