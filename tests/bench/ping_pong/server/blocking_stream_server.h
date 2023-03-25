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
 * \brief Definition of blocking_stream_server class.
 */
#pragma once

#include <atomic>
#include <string>
#include <thread>

#include "server_base.h"
#include "shm_stream/blocking_stream.h"

namespace shm_stream_test {

/*!
 * \brief Class of server using blocking streams.
 */
class blocking_stream_server : public server_base {
public:
    /*!
     * \brief Constructor.
     */
    blocking_stream_server();

    blocking_stream_server(const blocking_stream_server&) = delete;
    blocking_stream_server(blocking_stream_server&&) = delete;
    blocking_stream_server& operator=(const blocking_stream_server&) = delete;
    blocking_stream_server& operator=(blocking_stream_server&&) = delete;

    /*!
     * \brief Destructor.
     */
    ~blocking_stream_server() override;

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

    //! Input stream.
    shm_stream::blocking_stream_reader input_{};

    //! Output stream.
    shm_stream::blocking_stream_writer output_{};

    //! Thread to process communication.
    std::thread thread_{};
};

}  // namespace shm_stream_test
