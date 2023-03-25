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
 * \brief Test of streams of bytes without waiting (possibly lock-free).
 */
#include "shm_stream/light_stream.h"

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("shm_stream::light_stream_writer") {
    using shm_stream::light_stream_writer;
    using shm_stream::shm_stream_size_t;

    const std::string stream_name = "light_stream_writer_test";
    boost::interprocess::shared_memory_object::remove(
        ("shm_stream_light_stream_data_" + stream_name).c_str());
    boost::interprocess::named_mutex::remove(
        ("shm_stream_light_stream_lock_" + stream_name).c_str());

    SECTION("open a stream") {
        light_stream_writer writer;
        CHECK_FALSE(writer.is_opened());

        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.is_opened());
    }

    SECTION("move construct") {
        light_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.is_opened());

        light_stream_writer moved{std::move(writer)};
        CHECK(moved.is_opened());
        CHECK_FALSE(writer.is_opened());  // NOLINT
    }

    SECTION("close a stream explicitly") {
        light_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.is_opened());

        writer.close();

        CHECK_FALSE(writer.is_opened());
    }

    SECTION("get the available size") {
        light_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);

        CHECK(writer.available_size() == buffer_size - 1U);
    }

    SECTION("reserve with size") {
        light_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.available_size() == buffer_size - 1U);

        constexpr shm_stream_size_t expected_size = 5U;
        const auto buffer = writer.try_reserve(expected_size);

        CHECK(buffer.size() == expected_size);
    }

    SECTION("reserve with large size") {
        light_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.available_size() == buffer_size - 1U);

        constexpr shm_stream_size_t expected_size = 100U;
        const auto buffer = writer.try_reserve(expected_size);

        CHECK(buffer.size() == buffer_size - 1U);
    }

    SECTION("reserve bytes as many as possible") {
        light_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.available_size() == buffer_size - 1U);

        const auto buffer = writer.try_reserve();

        CHECK(buffer.size() == buffer_size - 1U);
    }

    SECTION("commit written bytes") {
        light_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.available_size() == buffer_size - 1U);
        (void)writer.try_reserve();

        writer.commit(3U);

        CHECK(writer.available_size() == buffer_size - 4U);  // NOLINT
    }

    SECTION("call functions for closed stream") {
        light_stream_writer writer;

        CHECK(writer.available_size() == 0U);
        CHECK(writer.try_reserve(1U).size() == 0U);  // NOLINT
        CHECK(writer.try_reserve().size() == 0U);    // NOLINT
        CHECK_NOTHROW(writer.commit(1U));
    }

    boost::interprocess::shared_memory_object::remove(
        ("shm_stream_light_stream_data_" + stream_name).c_str());
    boost::interprocess::named_mutex::remove(
        ("shm_stream_light_stream_lock_" + stream_name).c_str());
}

TEST_CASE("shm_stream::light_stream_reader") {
    using shm_stream::light_stream_reader;
    using shm_stream::light_stream_writer;
    using shm_stream::shm_stream_size_t;

    const std::string stream_name = "light_stream_reader_test";
    boost::interprocess::shared_memory_object::remove(
        ("shm_stream_light_stream_data_" + stream_name).c_str());
    boost::interprocess::named_mutex::remove(
        ("shm_stream_light_stream_lock_" + stream_name).c_str());

    SECTION("open a stream") {
        light_stream_reader reader;
        CHECK_FALSE(reader.is_opened());

        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);
        CHECK(reader.is_opened());
    }

    SECTION("move construct") {
        light_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);
        CHECK(reader.is_opened());

        light_stream_reader moved{std::move(reader)};
        CHECK(moved.is_opened());
        CHECK_FALSE(reader.is_opened());  // NOLINT
    }

    SECTION("close a stream explicitly") {
        light_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);
        CHECK(reader.is_opened());

        reader.close();

        CHECK_FALSE(reader.is_opened());
    }

    SECTION("get the available size") {
        light_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);
        light_stream_writer writer;
        writer.open(stream_name, buffer_size);
        (void)writer.try_reserve();
        writer.commit(3U);

        CHECK(reader.available_size() == 3U);
    }

    SECTION("reserve with size") {
        light_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);
        light_stream_writer writer;
        writer.open(stream_name, buffer_size);
        (void)writer.try_reserve();
        writer.commit(3U);

        constexpr shm_stream_size_t expected_size = 2U;
        const auto buffer = reader.try_reserve(expected_size);

        CHECK(buffer.size() == expected_size);
    }

    SECTION("reserve with large size") {
        light_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);
        light_stream_writer writer;
        writer.open(stream_name, buffer_size);
        (void)writer.try_reserve();
        writer.commit(3U);

        constexpr shm_stream_size_t expected_size = 100U;
        const auto buffer = reader.try_reserve(expected_size);

        CHECK(buffer.size() == 3U);
    }

    SECTION("reserve bytes as many as possible") {
        light_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);
        light_stream_writer writer;
        writer.open(stream_name, buffer_size);
        (void)writer.try_reserve();
        writer.commit(3U);

        const auto buffer = reader.try_reserve();

        CHECK(buffer.size() == 3U);
    }

    SECTION("commit read bytes") {
        light_stream_reader reader;
        constexpr shm_stream_size_t buffer_size = 10U;
        reader.open(stream_name, buffer_size);
        light_stream_writer writer;
        writer.open(stream_name, buffer_size);
        (void)writer.try_reserve();
        writer.commit(3U);
        (void)reader.try_reserve();

        reader.commit(2U);

        CHECK(reader.available_size() == 1U);
    }

    SECTION("call functions for closed stream") {
        light_stream_reader reader;

        CHECK(reader.available_size() == 0U);
        CHECK(reader.try_reserve(1U).size() == 0U);  // NOLINT
        CHECK(reader.try_reserve().size() == 0U);    // NOLINT
        CHECK_NOTHROW(reader.commit(1U));
    }

    boost::interprocess::shared_memory_object::remove(
        ("shm_stream_light_stream_data_" + stream_name).c_str());
    boost::interprocess::named_mutex::remove(
        ("shm_stream_light_stream_lock_" + stream_name).c_str());
}

TEST_CASE("shm_stream::light_stream") {
    using shm_stream::shm_stream_size_t;

    const std::string stream_name = "light_stream_reader_test";

    boost::interprocess::shared_memory_object::remove(
        ("shm_stream_light_stream_data_" + stream_name).c_str());
    boost::interprocess::named_mutex::remove(
        ("shm_stream_light_stream_lock_" + stream_name).c_str());

    SECTION("create a stream") {
        constexpr shm_stream_size_t buffer_size = 10U;
        shm_stream::light_stream::create(stream_name, buffer_size);

        CHECK(boost::interprocess::shared_memory_object::remove(
            ("shm_stream_light_stream_data_" + stream_name).c_str()));
        CHECK(boost::interprocess::named_mutex::remove(
            ("shm_stream_light_stream_lock_" + stream_name).c_str()));
    }

    SECTION("remove a stream") {
        constexpr shm_stream_size_t buffer_size = 10U;
        shm_stream::light_stream::create(stream_name, buffer_size);
        shm_stream::light_stream::remove(stream_name);

        CHECK_FALSE(boost::interprocess::shared_memory_object::remove(
            ("shm_stream_light_stream_data_" + stream_name).c_str()));
        CHECK_FALSE(boost::interprocess::named_mutex::remove(
            ("shm_stream_light_stream_lock_" + stream_name).c_str()));
    }

    boost::interprocess::shared_memory_object::remove(
        ("shm_stream_light_stream_data_" + stream_name).c_str());
    boost::interprocess::named_mutex::remove(
        ("shm_stream_light_stream_lock_" + stream_name).c_str());
}
