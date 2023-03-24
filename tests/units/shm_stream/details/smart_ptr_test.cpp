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
 * \brief Test of smart_ptr class.
 */
#include "shm_stream/details/smart_ptr.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("shm_stream::details::smart_ptr") {
    using shm_stream::details::smart_ptr;

    struct test_struct {
        bool destroyed{false};
    };
    test_struct test_obj{};

    const auto deleter = [](test_struct* ptr) {
        REQUIRE(static_cast<void*>(ptr) != nullptr);
        CHECK_FALSE(ptr->destroyed);
        ptr->destroyed = true;
    };

    SECTION("destroy object in destructor") {
        {
            smart_ptr<test_struct> ptr{&test_obj, deleter};

            CHECK_FALSE(test_obj.destroyed);
        }
        CHECK(test_obj.destroyed);
    }

    SECTION("destroy object in reset function") {
        smart_ptr<test_struct> ptr{&test_obj, deleter};
        CHECK_FALSE(test_obj.destroyed);

        ptr.reset();

        CHECK(test_obj.destroyed);
    }

    SECTION("get pointer") {
        smart_ptr<test_struct> ptr{&test_obj, deleter};

        CHECK(static_cast<void*>(ptr.get()) == static_cast<void*>(&test_obj));
    }

    SECTION("check existence of the object") {
        smart_ptr<test_struct> ptr{&test_obj, deleter};

        CHECK(ptr.has_obj());
    }

    SECTION("default constructor") {
        smart_ptr<test_struct> ptr;

        CHECK(static_cast<void*>(ptr.get()) == nullptr);
        CHECK_FALSE(ptr.has_obj());
    }

    SECTION("move constructor") {
        smart_ptr<test_struct> ptr1{&test_obj, deleter};

        smart_ptr<test_struct> ptr2{std::move(ptr1)};

        CHECK(static_cast<void*>(ptr1.get()) == nullptr);  // NOLINT
        CHECK(static_cast<void*>(ptr2.get()) == static_cast<void*>(&test_obj));
        CHECK_FALSE(test_obj.destroyed);
    }

    SECTION("move assignment operator") {
        smart_ptr<test_struct> ptr1{&test_obj, deleter};

        smart_ptr<test_struct> ptr2;
        ptr2 = std::move(ptr1);

        CHECK(static_cast<void*>(ptr1.get()) == nullptr);  // NOLINT
        CHECK(static_cast<void*>(ptr2.get()) == static_cast<void*>(&test_obj));
        CHECK_FALSE(test_obj.destroyed);
    }
}
