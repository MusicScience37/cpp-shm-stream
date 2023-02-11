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
 * \brief Class of exceptions thrown for invalid function arguments.
 */
class invalid_argument : public shm_stream_exception {
public:
    using shm_stream_exception::shm_stream_exception;
    using shm_stream_exception::what;
};

}  // namespace shm_stream
