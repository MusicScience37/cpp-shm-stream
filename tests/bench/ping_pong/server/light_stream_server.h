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
 * \brief Definition of light_stream_server class.
 */
#pragma once

#include <atomic>
#include <string>
#include <thread>

#include "server_base.h"
#include "shm_stream/light_stream.h"

namespace shm_stream_test {

/*!
 * \brief Class of server using light streams.
 */
class light_stream_server : public shm_stream_test::server_base {
public:
    /*!
     * \brief Constructor.
     */
    light_stream_server();

    light_stream_server(const light_stream_server&) = delete;
    light_stream_server(light_stream_server&&) = delete;
    light_stream_server& operator=(const light_stream_server&) = delete;
    light_stream_server& operator=(light_stream_server&&) = delete;

    /*!
     * \brief Destructor.
     */
    ~light_stream_server() override;

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
    shm_stream::light_stream_reader input_{};

    //! Output stream.
    shm_stream::light_stream_writer output_{};

    //! Thread to process communication.
    std::thread thread_{};

    //! Flag to stop processing.
    std::atomic<bool> is_stopped_{false};
};

}  // namespace shm_stream_test
