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
 * \brief Test of C_SHM_STREAM_TRANSLATE_ERROR macro.
 */
#include "shm_stream/c_interface/translate_error.h"

#include <stdexcept>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "shm_stream/c_interface/error_codes.h"
#include "shm_stream/shm_stream_exception.h"

TEST_CASE("C_SHM_STREAM_TRANSLATE_ERROR") {
    SECTION("return success without error") {
        const auto null_func = [] {};
        const auto c_func = [&null_func] {
            C_SHM_STREAM_TRANSLATE_ERROR(null_func());
        };

        const c_shm_stream_error_code_t code = c_func();

        CHECK(code == c_shm_stream_error_code_success);
    }

    SECTION("catch shm_stream_error") {
        const c_shm_stream_error_code_t expected_code =
            // NOLINTNEXTLINE
            GENERATE(c_shm_stream_error_code_invalid_argument,
                c_shm_stream_error_code_failed_to_open,
                c_shm_stream_error_code_internal_error);
        const auto c_func = [expected_code] {
            C_SHM_STREAM_TRANSLATE_ERROR(
                throw shm_stream::shm_stream_error(expected_code));
        };

        const c_shm_stream_error_code_t code = c_func();

        CHECK(code == expected_code);
    }

    SECTION("catch other exceptions") {
        const auto c_func = [] {
            C_SHM_STREAM_TRANSLATE_ERROR(
                throw std::runtime_error("Test exception."));
        };

        const c_shm_stream_error_code_t code = c_func();

        CHECK(code == c_shm_stream_error_code_internal_error);
    }
}
