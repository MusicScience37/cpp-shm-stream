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

#include <catch2/catch_test_macros.hpp>

TEST_CASE("no_wait_stream_writer, no_wait_stream_reader") {
    using shm_stream::no_wait_stream_writer;
    // TODO
}
