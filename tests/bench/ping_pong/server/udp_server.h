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
 * \brief Definition of udp_server class.
 */
#pragma once

#include <cstddef>
#include <thread>
#include <vector>

#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>

#include "../common.h"
#include "server_base.h"

namespace shm_stream_test {

/*!
 * \brief Class of UDP servers.
 *
 */
class udp_server : public server_base {
public:
    /*!
     * \brief Constructor.
     */
    explicit udp_server(protocol_type protocol);

    udp_server(const udp_server&) = delete;
    udp_server(udp_server&&) = delete;
    udp_server& operator=(const udp_server&) = delete;
    udp_server& operator=(udp_server&&) = delete;

    /*!
     * \brief Destructor.
     */
    ~udp_server() override;

    /*!
     * \brief Start processing.
     */
    void start() override;

    /*!
     * \brief Stop processing.
     */
    void stop() override;

private:
    /*!
     * \brief Process communication.
     */
    void run();

    /*!
     * \brief Receive next data asynchronously.
     */
    void async_receive_next();

    /*!
     * \brief Process received data.
     *
     * \param[in] bytes_transferred Number of bytes transferred.
     */
    void on_receive(std::size_t bytes_transferred);

    //! Context.
    asio::io_context context_{1};

    //! Socket.
    asio::ip::udp::socket socket_;

    //! Sender endpoint.
    asio::ip::udp::endpoint sender_endpoint_;

    //! Buffer of data.
    std::vector<char> buffer_;

    //! Thread.
    std::thread thread_;
};

}  // namespace shm_stream_test
