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
 * \brief Test of error codes.
 */
#include "shm_stream/c_interface/error_codes.h"

#include <string>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("c_shm_stream_error_message") {
    SECTION("get messages") {
        const auto to_message =
            [](c_shm_stream_error_code_t code) -> std::string {
            return c_shm_stream_error_message(code);
        };

        CHECK(to_message(c_shm_stream_error_code_success) == "Success.");
        CHECK(to_message(c_shm_stream_error_code_invalid_argument) ==
            "Invalid argument.");
        CHECK(to_message(c_shm_stream_error_code_failed_to_open) ==
            "Failed to create or open a stream.");
        CHECK(to_message(c_shm_stream_error_code_internal_error) ==
            "Internal error.");
        CHECK(to_message(static_cast<c_shm_stream_error_code_t>(
                  c_shm_stream_error_code_internal_error + 1)) ==
            "Invalid error code.");
    }
}
