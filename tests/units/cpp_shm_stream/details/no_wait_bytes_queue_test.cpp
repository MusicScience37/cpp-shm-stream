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
 * \brief Test of no_wait_bytes_queue class.
 */
#include "shm_stream/details/no_wait_bytes_queue.h"

#include <array>

#include <boost/atomic/ipc_atomic.hpp>
#include <catch2/catch_test_macros.hpp>

#include "shm_stream/bytes_view.h"
#include "shm_stream/common_types.h"
#include "shm_stream/details/atomic_index_pair.h"

TEST_CASE("shm_stream::details::no_wait_bytes_queue_writer") {
    using shm_stream::mutable_bytes_view;
    using shm_stream::shm_stream_size_t;
    using shm_stream::details::no_wait_bytes_queue_writer;

    using atomic_type = boost::atomics::ipc_atomic<shm_stream_size_t>;
    using atomic_index_pair_type =
        shm_stream::details::atomic_index_pair<atomic_type>;
    using atomic_index_pair_view_type =
        shm_stream::details::atomic_index_pair_view<atomic_type>;
    using writer_type = no_wait_bytes_queue_writer<atomic_type>;

    SECTION("check size in constructor") {
        atomic_index_pair_type indices;
        char dummy_buffer{};
        const auto try_create = [&indices, &dummy_buffer](
                                    shm_stream_size_t size) {
            (void)writer_type(indices, mutable_bytes_view(&dummy_buffer, size));
        };

        CHECK_THROWS(try_create(0U));
        CHECK_THROWS(try_create(1U));
        CHECK_NOTHROW(try_create(2U));
        CHECK_NOTHROW(try_create(0x7FFFFFFFU));
        CHECK_THROWS(try_create(0x80000000U));
    }

    SECTION("get available size") {
        atomic_type read_index{0U};
        atomic_type write_index{0U};
        constexpr shm_stream_size_t buffer_size = 7U;
        std::array<char, buffer_size> raw_buffer{};

        SECTION("when no byte is written") {
            read_index = 1U;
            write_index = 1U;
            writer_type writer{
                atomic_index_pair_view_type(&write_index, &read_index),
                mutable_bytes_view(raw_buffer.data(), buffer_size)};

            CHECK(writer.available_size() == 6U);  // NOLINT
        }

        SECTION("when one byte is written") {
            read_index = 1U;
            write_index = 2U;
            writer_type writer{
                atomic_index_pair_view_type(&write_index, &read_index),
                mutable_bytes_view(raw_buffer.data(), buffer_size)};

            CHECK(writer.available_size() == 5U);  // NOLINT
        }

        SECTION("when some bytes are written") {
            read_index = 2U;
            write_index = 6U;  // NOLINT
            writer_type writer{
                atomic_index_pair_view_type(&write_index, &read_index),
                mutable_bytes_view(raw_buffer.data(), buffer_size)};

            CHECK(writer.available_size() == 2U);  // NOLINT
        }

        SECTION("when full") {
            read_index = 2U;
            write_index = 1U;  // NOLINT
            writer_type writer{
                atomic_index_pair_view_type(&write_index, &read_index),
                mutable_bytes_view(raw_buffer.data(), buffer_size)};

            CHECK(writer.available_size() == 0U);  // NOLINT
        }
    }

    SECTION("reserve bytes") {
        atomic_type read_index{0U};
        atomic_type write_index{0U};
        constexpr shm_stream_size_t buffer_size = 7U;
        std::array<char, buffer_size> raw_buffer{};

        SECTION("when no byte is written and expected size is small") {
            read_index = 1U;
            write_index = 1U;
            writer_type writer{
                atomic_index_pair_view_type(&write_index, &read_index),
                mutable_bytes_view(raw_buffer.data(), buffer_size)};

            const auto buffer = writer.try_reserve(3U);  // NOLINT

            CHECK(buffer.data() - raw_buffer.data() == 1U);
            CHECK(buffer.size() == 3U);  // NOLINT
        }

        SECTION("when no byte is written and expected size is large") {
            read_index = 1U;
            write_index = 1U;
            writer_type writer{
                atomic_index_pair_view_type(&write_index, &read_index),
                mutable_bytes_view(raw_buffer.data(), buffer_size)};

            const auto buffer = writer.try_reserve(10U);  // NOLINT

            CHECK(buffer.data() - raw_buffer.data() == 1U);
            CHECK(buffer.size() == 6U);  // NOLINT
        }

        SECTION("when the queue is in the initial state") {
            read_index = 0U;
            write_index = 0U;
            writer_type writer{
                atomic_index_pair_view_type(&write_index, &read_index),
                mutable_bytes_view(raw_buffer.data(), buffer_size)};

            const auto buffer = writer.try_reserve(10U);  // NOLINT

            CHECK(buffer.data() - raw_buffer.data() == 0U);
            CHECK(buffer.size() == 6U);  // NOLINT
        }

        SECTION("when one byte is written after the initialization") {
            read_index = 0U;
            write_index = 1U;
            writer_type writer{
                atomic_index_pair_view_type(&write_index, &read_index),
                mutable_bytes_view(raw_buffer.data(), buffer_size)};

            const auto buffer = writer.try_reserve(10U);  // NOLINT

            CHECK(buffer.data() - raw_buffer.data() == 1U);
            CHECK(buffer.size() == 5U);  // NOLINT
        }

        SECTION("when one byte is written and one byte is read") {
            read_index = 1U;
            write_index = 2U;
            writer_type writer{
                atomic_index_pair_view_type(&write_index, &read_index),
                mutable_bytes_view(raw_buffer.data(), buffer_size)};

            const auto buffer = writer.try_reserve(10U);  // NOLINT

            CHECK(buffer.data() - raw_buffer.data() == 2U);
            CHECK(buffer.size() == 5U);  // NOLINT
        }

        SECTION("when some bytes are written") {
            read_index = 2U;
            write_index = 6U;  // NOLINT
            writer_type writer{
                atomic_index_pair_view_type(&write_index, &read_index),
                mutable_bytes_view(raw_buffer.data(), buffer_size)};

            const auto buffer = writer.try_reserve(10U);  // NOLINT

            CHECK(buffer.data() - raw_buffer.data() == 6U);
            CHECK(buffer.size() == 1U);  // NOLINT
        }

        SECTION("when full") {
            read_index = 2U;
            write_index = 1U;  // NOLINT
            writer_type writer{
                atomic_index_pair_view_type(&write_index, &read_index),
                mutable_bytes_view(raw_buffer.data(), buffer_size)};

            const auto buffer = writer.try_reserve(10U);  // NOLINT

            CHECK(buffer.data() - raw_buffer.data() == 1U);
            CHECK(buffer.size() == 0U);  // NOLINT
        }
    }

    SECTION("commit bytes") {
        atomic_type read_index{0U};
        atomic_type write_index{0U};
        constexpr shm_stream_size_t buffer_size = 7U;
        std::array<char, buffer_size> raw_buffer{};

        SECTION("when no byte is written") {
            read_index = 1U;
            write_index = 1U;
            writer_type writer{
                atomic_index_pair_view_type(&write_index, &read_index),
                mutable_bytes_view(raw_buffer.data(), buffer_size)};
            const auto buffer = writer.try_reserve(1U);
            CHECK(buffer.size() == 1U);

            writer.commit(0U);

            CHECK(read_index == 1U);
            CHECK(write_index == 1U);
        }

        SECTION("when some bytes are written") {
            read_index = 1U;
            write_index = 1U;
            writer_type writer{
                atomic_index_pair_view_type(&write_index, &read_index),
                mutable_bytes_view(raw_buffer.data(), buffer_size)};
            const auto buffer = writer.try_reserve(3U);
            CHECK(buffer.size() == 3U);

            writer.commit(2U);

            CHECK(read_index == 1U);
            CHECK(write_index == 3U);
        }

        SECTION("when all reserved bytes are written") {
            read_index = 2U;
            write_index = 4U;
            writer_type writer{
                atomic_index_pair_view_type(&write_index, &read_index),
                mutable_bytes_view(raw_buffer.data(), buffer_size)};
            const auto buffer = writer.try_reserve(3U);
            CHECK(buffer.size() == 3U);

            writer.commit(3U);

            CHECK(read_index == 2U);
            CHECK(write_index == 0U);
        }
    }
}
