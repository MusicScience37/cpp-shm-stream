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
 * \brief Definition of common functions.
 */
#pragma once

#include <cstdint>
#include <string>

#include "shm_stream/common_types.h"

namespace shm_stream_test {

//! Enumeration of protocol types.
enum class protocol_type {
    //! Light streams.
    light_stream,

    //! Blocking streams.
    blocking_stream,

    //! UDP in IPv4.
    udp_v4,

    //! UDP in IPv6.
    udp_v6
};

/*!
 * \brief Get the port number of the command server.
 *
 * \return Port number.
 */
inline constexpr std::uint16_t command_port() {
    // NOLINTNEXTLINE
    return 23456;
}

/*!
 * \brief Get the size of buffers.
 *
 * \return Size of buffers.
 */
inline constexpr shm_stream::shm_stream_size_t buffer_size() {
    // NOLINTNEXTLINE
    return 10 * 1024 * 1024;  // 10 MB
}

/*!
 * \brief Get the name of a stream of requests.
 *
 * \return Name of the stream.
 */
inline std::string request_stream_name() {
    return "shm_stream_bench_ping_pong_request";
}

/*!
 * \brief Get the name of a stream of responses.
 *
 * \return Name of the stream.
 */
inline std::string response_stream_name() {
    return "shm_stream_bench_ping_pong_response";
}

/*!
 * \brief Get the port number for benchmark of UDP.
 *
 * \return Port number.
 */
inline std::uint16_t udp_port() {
    // NOLINTNEXTLINE
    return static_cast<std::uint16_t>(12345);
}

}  // namespace shm_stream_test
