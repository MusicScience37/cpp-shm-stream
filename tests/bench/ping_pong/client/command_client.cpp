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
 * \brief Implementation of command_client class.
 */
#include "command_client.h"

#include <memory>

#include <rpc/client.h>

namespace shm_stream_test {

command_client::command_client() {
    command_client_ =
        std::make_unique<rpc::client>("127.0.0.1", command_port());
}

command_client::~command_client() = default;

void command_client::change_protocol(protocol_type protocol) {
    command_client_->call("change_protocol", static_cast<int>(protocol));
}

}  // namespace shm_stream_test
