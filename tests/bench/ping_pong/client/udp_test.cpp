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
 * \brief Benchmark of UDP.
 */
#include <asio/ip/udp.hpp>
#include <asio/registered_buffer.hpp>
#include <stat_bench/benchmark_macros.h>

#include "../common.h"
#include "command_client.h"
#include "ping_pong_fixture.h"

STAT_BENCH_CASE_F(shm_stream_test::ping_pong_fixture, "ping_pong", "UDPv4") {
    shm_stream_test::command_client().change_protocol(
        shm_stream_test::protocol_type::udp_v4);

    const std::string& data = this->get_data();
    const std::size_t data_size = data.size();

    const std::uint16_t server_port = shm_stream_test::udp_port();
    const auto server_endpoint =
        asio::ip::udp::endpoint(asio::ip::address_v4::loopback(), server_port);

    asio::io_context client_context{1};
    asio::ip::udp::socket client_socket{client_context};
    client_socket.connect(server_endpoint);

    std::vector<char> received_data(data_size);

    STAT_BENCH_MEASURE() {
        client_socket.send(asio::const_buffer(data.data(), data.size()));
        client_socket.receive(
            asio::buffer(received_data.data(), received_data.size()));
    };
}

STAT_BENCH_CASE_F(shm_stream_test::ping_pong_fixture, "ping_pong", "UDPv6") {
    shm_stream_test::command_client().change_protocol(
        shm_stream_test::protocol_type::udp_v6);

    const std::string& data = this->get_data();
    const std::size_t data_size = data.size();

    const std::uint16_t server_port = shm_stream_test::udp_port();
    const auto server_endpoint =
        asio::ip::udp::endpoint(asio::ip::address_v6::loopback(), server_port);

    asio::io_context client_context{1};
    asio::ip::udp::socket client_socket{client_context};
    client_socket.connect(server_endpoint);

    std::vector<char> received_data(data_size);

    STAT_BENCH_MEASURE() {
        client_socket.send(asio::const_buffer(data.data(), data.size()));
        client_socket.receive(
            asio::buffer(received_data.data(), received_data.size()));
    };
}
