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
 * \brief Definition of exceptions.
 */
#pragma once

#include <stdexcept>

#include "shm_stream/c_interface/error_codes.h"

namespace shm_stream {

/*!
 * \brief Class of exceptions in cpp-shm-stream library.
 */
class shm_stream_exception : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
    using std::runtime_error::what;
};

/*!
 * \brief Class of errors using error codes.
 */
class shm_stream_error : public shm_stream_exception {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] code Error code.
     */
    explicit shm_stream_error(c_shm_stream_error_code_t code)
        : shm_stream_exception(c_shm_stream_error_message(code)), code_(code) {}

    using shm_stream_exception::what;

    /*!
     * \brief Get the error code.
     *
     * \return Error code.
     */
    [[nodiscard]] c_shm_stream_error_code_t code() const noexcept {
        return code_;
    }

private:
    //! Error code.
    c_shm_stream_error_code_t code_;
};

}  // namespace shm_stream
