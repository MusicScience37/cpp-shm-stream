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
 * \brief Definition of server_base class.
 */
#pragma once

namespace shm_stream_test {

/*!
 * \brief Interface of server.
 */
class server_base {
public:
    /*!
     * \brief Start processing.
     */
    virtual void start() = 0;

    /*!
     * \brief Stop processing.
     */
    virtual void stop() = 0;

    //! Constructor.
    server_base() = default;

    server_base(const server_base&) = delete;
    server_base(server_base&&) = delete;
    server_base& operator=(const server_base&) = delete;
    server_base& operator=(server_base&&) = delete;

    //! Destructor.
    virtual ~server_base() = default;
};

}  // namespace shm_stream_test
