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
 * \brief Benchmark of streams of bytes without waiting.
 */
#include <cstdint>
#include <thread>
#include <vector>

#include <boost/asio/buffer.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/address_v6.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/system/detail/error_code.hpp>
#include <stat_bench/benchmark_macros.h>

#include "send_messages_fixture.h"

class udp_server {
public:
    udp_server(const boost::asio::ip::udp::endpoint& server_endpoint,
        std::size_t data_size)
        : socket_(context_, server_endpoint), buffer_(data_size) {
        thread_ = std::thread{[this] { run(); }};
    }

    ~udp_server() {
        context_.stop();
        thread_.join();
    }

    udp_server(const udp_server&) = delete;
    udp_server(udp_server&&) = delete;
    udp_server& operator=(const udp_server&) = delete;
    udp_server& operator=(udp_server&&) = delete;

private:
    void run() {
        boost::asio::dispatch(context_, [this] { this->async_receive_next(); });
        context_.run();
    }

    void stop() { context_.stop(); }

    void async_receive_next() {
        socket_.async_receive(
            boost::asio::buffer(buffer_.data(), buffer_.size()),
            [this](const boost::system::error_code& /*code*/,
                std::size_t /*bytes_transferred*/) {
                this->async_receive_next();
            });
    }

    //! Context.
    boost::asio::io_context context_{1};

    //! Socket.
    boost::asio::ip::udp::socket socket_;

    //! Buffer of data.
    std::vector<char> buffer_;

    //! Thread.
    std::thread thread_;
};

STAT_BENCH_CASE_F(
    shm_stream_test::send_messages_fixture, "send_messages", "UDPv4") {
    const std::string& data = this->get_data();
    const std::size_t data_size = data.size();

    const std::uint16_t server_port = 12345;
    const auto server_endpoint = boost::asio::ip::udp::endpoint(
        boost::asio::ip::address_v4::loopback(), server_port);

    udp_server server{server_endpoint, data_size};

    boost::asio::io_context client_context{1};
    boost::asio::ip::udp::socket client_socket{client_context};
    client_socket.connect(server_endpoint);

    STAT_BENCH_MEASURE() {
        client_socket.send(boost::asio::const_buffer(data.data(), data.size()));
    };
}

STAT_BENCH_CASE_F(
    shm_stream_test::send_messages_fixture, "send_messages", "UDPv6") {
    const std::string& data = this->get_data();
    const std::size_t data_size = data.size();

    const std::uint16_t server_port = 12345;
    const auto server_endpoint = boost::asio::ip::udp::endpoint(
        boost::asio::ip::address_v6::loopback(), server_port);

    udp_server server{server_endpoint, data_size};

    boost::asio::io_context client_context{1};
    boost::asio::ip::udp::socket client_socket{client_context};
    client_socket.connect(server_endpoint);

    STAT_BENCH_MEASURE() {
        client_socket.send(boost::asio::const_buffer(data.data(), data.size()));
    };
}
