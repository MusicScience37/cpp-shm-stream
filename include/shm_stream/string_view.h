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

#include <fmt/format.h>

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
     * \param[in] data Pointer to the data.
     */
    constexpr string_view(  // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
        const char* data) noexcept
        : data_(data), size_(len(data)) {}

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

    /*!
     * \brief Convert to std::string.
     *
     * \return This string converted to std::string.
     */
    explicit operator std::string() const { return std::string(data_, size_); }

private:
    //! Pointer to the data.
    const char* data_;

    //! Size of the data.
    std::size_t size_;

    /*!
     * \brief Get the length of a string.
     *
     * \param[in] data Pointer to the data of the string.
     * \return Length of the string.
     */
    [[nodiscard]] static constexpr std::size_t len(const char* data) noexcept {
        const char* end = data;
        while (*end != '\0') {
            ++end;
        }
        return end - data;
    }
};

}  // namespace shm_stream

namespace fmt {

/*!
 * \brief Specialization of fmt::formatter for shm_stream::string_view.
 */
template <>
struct formatter<shm_stream::string_view> : public formatter<fmt::string_view> {
public:
    /*!
     * \brief Format a value.
     *
     * \tparam FormatContext Type of the context.
     * \param[in] val Value.
     * \param[in] ctx Context.
     * \return Output iterator after the format.
     */
    template <typename FormatContext>
    auto format(const shm_stream::string_view& val, FormatContext& ctx) const
        -> decltype(ctx.out()) {
        return formatter<fmt::string_view>::format(
            fmt::string_view(val.data(), val.size()), ctx);
    }
};

}  // namespace fmt
