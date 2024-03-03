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
 * \brief Definition of command_client class.
 */
#pragma once

#include <memory>

#include "../common.h"

namespace httplib {

class Client;

}  // namespace httplib

namespace shm_stream_test {

/*!
 * \brief Client of commands to control servers for benchmarks.
 */
class command_client {
public:
    /*!
     * \brief Constructor.
     */
    explicit command_client();

    command_client(const command_client&) = delete;
    command_client(command_client&&) = delete;
    command_client& operator=(const command_client&) = delete;
    command_client& operator=(command_client&&) = delete;

    /*!
     * \brief Destructor.
     */
    ~command_client();

    /*!
     * \brief Change protocol.
     *
     * \param[in] protocol Protocol.
     */
    void change_protocol(protocol_type protocol);

private:
    //! Command client.
    std::unique_ptr<httplib::Client> command_client_{};
};

}  // namespace shm_stream_test
