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
 * \brief Test of atomic_index_pair class.
 */
#include "shm_stream/details/atomic_index_pair.h"

#include <catch2/catch_test_macros.hpp>

#include "shm_stream/details/cache_line_size.h"

TEST_CASE("shm_stream::details::atomic_index_pair") {
    using shm_stream::details::atomic_index_pair;
    using shm_stream::details::cache_line_size;

    SECTION("has proper alignment") {
        STATIC_CHECK(alignof(atomic_index_pair<>) == cache_line_size());
        STATIC_CHECK(sizeof(atomic_index_pair<>) == 2U * cache_line_size());
    }
}
