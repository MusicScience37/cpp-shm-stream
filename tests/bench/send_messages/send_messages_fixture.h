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
 * \brief Definition of send_messages_fixture class.
 */
#pragma once

#include <cstddef>

#include <stat_bench/fixture_base.h>
#include <stat_bench/invocation_context.h>

#include "shm_stream_test/generate_data.h"

namespace shm_stream_test {

class send_messages_fixture : public stat_bench::FixtureBase {
public:
    send_messages_fixture() {
        this->add_param<std::size_t>("size")
            ->add(32)         // NOLINT
            ->add(1024)       // NOLINT
            ->add(32 * 1024)  // NOLINT
#ifdef NDEBUG
            ->add(1024 * 1024)  // NOLINT
#endif
            ;
    }

    void setup(stat_bench::InvocationContext& context) override {
        size_ = context.get_param<std::size_t>("size");
        data_ = generate_data(size_);
    }

    [[nodiscard]] const std::string& get_data() const noexcept { return data_; }

private:
    //! Number of bytes.
    std::size_t size_{0};

    //! Data.
    std::string data_{};
};

}  // namespace shm_stream_test
