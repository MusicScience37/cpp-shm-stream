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
 * \brief Implementation of udp_server class.
 */
#include "udp_server.h"

#include <cstddef>
#include <cstdlib>

#include <asio/buffer.hpp>
#include <asio/dispatch.hpp>
#include <asio/error_code.hpp>
#include <asio/ip/address_v4.hpp>
#include <asio/ip/address_v6.hpp>
#include <asio/ip/udp.hpp>
#include <asio/registered_buffer.hpp>

#include "../common.h"

namespace shm_stream_test {

static asio::ip::udp::endpoint server_endpoint(protocol_type protocol) {
    switch (protocol) {
    case protocol_type::udp_v4:
        return asio::ip::udp::endpoint(
            asio::ip::address_v4::loopback(), udp_port());
    case protocol_type::udp_v6:
        return asio::ip::udp::endpoint(
            asio::ip::address_v6::loopback(), udp_port());
    default:
        std::abort();
    }
}

udp_server::udp_server(protocol_type protocol)
    : socket_(context_, server_endpoint(protocol)), buffer_(buffer_size()) {
    thread_ = std::thread{[this] { run(); }};
}

udp_server::~udp_server() {
    context_.stop();
    if (thread_.joinable()) {
        thread_.join();
    }
}

void udp_server::start() {
    // No operation.
}

void udp_server::stop() {
    // No operation.
}

void udp_server::run() {
    asio::dispatch(context_, [this] { this->async_receive_next(); });
    context_.run();
}

void udp_server::async_receive_next() {
    socket_.async_receive_from(asio::buffer(buffer_.data(), buffer_.size()),
        sender_endpoint_,
        [this](
            const asio::error_code& /*code*/, std::size_t bytes_transferred) {
            this->on_receive(bytes_transferred);
        });
}

void udp_server::on_receive(std::size_t bytes_transferred) {
    socket_.send_to(asio::const_buffer(buffer_.data(), bytes_transferred),
        sender_endpoint_);
    async_receive_next();
}

}  // namespace shm_stream_test
