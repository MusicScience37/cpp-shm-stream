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
 * \brief Implementation of command_server class.
 */
#include "command_server.h"

#include <memory>

#include <rpc/server.h>

namespace shm_stream_test {

command_server::command_server(
    std::unordered_map<protocol_type, std::shared_ptr<server_base>>
        bench_server)
    : bench_server_(std::move(bench_server)) {
    command_server_ =
        std::make_unique<rpc::server>("127.0.0.1", command_port());

    command_server_->bind("change_protocol", [this](int protocol_number) {
        this->change_protocol(static_cast<protocol_type>(protocol_number));
    });

    command_server_->async_run();
}

command_server::~command_server() { command_server_->stop(); }

void command_server::change_protocol(protocol_type protocol) {
    for (const auto& pair : bench_server_) {
        pair.second->stop();
    }
    bench_server_.at(protocol)->start();
}

}  // namespace shm_stream_test
