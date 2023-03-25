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
 * \brief Definition of command_server class.
 */
#pragma once

#include <memory>
#include <unordered_map>

#include "../common.h"
#include "server_base.h"

namespace rpc {

class server;

}  // namespace rpc

namespace shm_stream_test {

/*!
 * \brief Server of commands to control servers for benchmarks.
 */
class command_server {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] bench_server Benchmark server.
     */
    explicit command_server(
        std::unordered_map<protocol_type, std::shared_ptr<server_base>>
            bench_server);

    command_server(const command_server&) = delete;
    command_server(command_server&&) = delete;
    command_server& operator=(const command_server&) = delete;
    command_server& operator=(command_server&&) = delete;

    /*!
     * \brief Destructor.
     */
    ~command_server();

private:
    /*!
     * \brief Change protocol.
     *
     * \param[in] protocol Protocol.
     */
    void change_protocol(protocol_type protocol);

    //! Command server.
    std::unique_ptr<rpc::server> command_server_{};

    //! Benchmark server.
    std::unordered_map<protocol_type, std::shared_ptr<server_base>>
        bench_server_;
};

}  // namespace shm_stream_test
