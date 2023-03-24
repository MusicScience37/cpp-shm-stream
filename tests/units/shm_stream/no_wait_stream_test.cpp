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
#include "shm_stream/no_wait_stream.h"

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("shm_stream::no_wait_stream_writer") {
    using shm_stream::no_wait_stream_writer;
    using shm_stream::shm_stream_size_t;

    const std::string stream_name = "no_wait_stream_writer_test";
    boost::interprocess::shared_memory_object::remove(
        ("shm_stream_light_stream_data_" + stream_name).c_str());
    boost::interprocess::named_mutex::remove(
        ("shm_stream_light_stream_lock_" + stream_name).c_str());

    SECTION("open a stream") {
        no_wait_stream_writer writer;
        CHECK_FALSE(writer.is_opened());

        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.is_opened());
    }

    SECTION("move construct") {
        no_wait_stream_writer writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.is_opened());

        no_wait_stream_writer moved{std::move(writer)};
        CHECK(moved.is_opened());
        CHECK_FALSE(writer.is_opened());  // NOLINT
    }

    boost::interprocess::shared_memory_object::remove(
        ("shm_stream_light_stream_data_" + stream_name).c_str());
    boost::interprocess::named_mutex::remove(
        ("shm_stream_light_stream_lock_" + stream_name).c_str());
}

TEST_CASE("shm_stream::no_wait_stream_reader") {
    using shm_stream::no_wait_stream_reader;
    using shm_stream::shm_stream_size_t;

    const std::string stream_name = "no_wait_stream_reader_test";
    boost::interprocess::shared_memory_object::remove(
        ("shm_stream_light_stream_data_" + stream_name).c_str());
    boost::interprocess::named_mutex::remove(
        ("shm_stream_light_stream_lock_" + stream_name).c_str());

    SECTION("open a stream") {
        no_wait_stream_reader writer;
        CHECK_FALSE(writer.is_opened());

        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.is_opened());
    }

    SECTION("move construct") {
        no_wait_stream_reader writer;
        constexpr shm_stream_size_t buffer_size = 10U;
        writer.open(stream_name, buffer_size);
        CHECK(writer.is_opened());

        no_wait_stream_reader moved{std::move(writer)};
        CHECK(moved.is_opened());
        CHECK_FALSE(writer.is_opened());  // NOLINT
    }

    boost::interprocess::shared_memory_object::remove(
        ("shm_stream_light_stream_data_" + stream_name).c_str());
    boost::interprocess::named_mutex::remove(
        ("shm_stream_light_stream_lock_" + stream_name).c_str());
}

TEST_CASE("shm_stream::no_wait_stream") {
    using shm_stream::shm_stream_size_t;

    const std::string stream_name = "no_wait_stream_reader_test";

    boost::interprocess::shared_memory_object::remove(
        ("shm_stream_light_stream_data_" + stream_name).c_str());
    boost::interprocess::named_mutex::remove(
        ("shm_stream_light_stream_lock_" + stream_name).c_str());

    SECTION("create a stream") {
        constexpr shm_stream_size_t buffer_size = 10U;
        shm_stream::no_wait_stream::create(stream_name, buffer_size);

        CHECK(boost::interprocess::shared_memory_object::remove(
            ("shm_stream_light_stream_data_" + stream_name).c_str()));
        CHECK(boost::interprocess::named_mutex::remove(
            ("shm_stream_light_stream_lock_" + stream_name).c_str()));
    }

    SECTION("remove a stream") {
        constexpr shm_stream_size_t buffer_size = 10U;
        shm_stream::no_wait_stream::create(stream_name, buffer_size);
        shm_stream::no_wait_stream::remove(stream_name);

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
