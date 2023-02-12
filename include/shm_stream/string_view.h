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
 * \brief Definition of string_view class.
 */
#pragma once

#include <cstddef>
#include <string>

namespace shm_stream {

/*!
 * \brief Class of views of strings.
 */
class string_view {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] data Pointer to the data.
     * \param[in] size Size of the data.
     */
    constexpr string_view(const char* data, std::size_t size) noexcept
        : data_(data), size_(size) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] data Data.
     */
    string_view(  // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
        const std::string& data) noexcept
        : string_view(data.data(), data.size()) {}

    /*!
     * \brief Get the data.
     *
     * \return Pointer to the data.
     */
    constexpr const char* data() const noexcept { return data_; }

    /*!
     * \brief Get the size.
     *
     * \return Size of the data.
     */
    constexpr std::size_t size() const noexcept { return size_; }

private:
    //! Pointer to the data.
    const char* data_;

    //! Size of the data.
    std::size_t size_;
};

}  // namespace shm_stream
