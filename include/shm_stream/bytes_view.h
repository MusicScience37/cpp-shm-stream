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
 * \brief Definition of bytes_view class.
 */
#pragma once

#include "shm_stream/common_types.h"

namespace shm_stream {

/*!
 * \brief Class of views of non-constant byte sequences.
 */
class mutable_bytes_view {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] data Pointer to the data.
     * \param[in] size Size of the data.
     */
    constexpr mutable_bytes_view(char* data, shm_stream_size_t size) noexcept
        : data_(data), size_(size) {}

    /*!
     * \brief Get the pointer to the data.
     *
     * \return Pointer to the data.
     */
    [[nodiscard]] char* data() const noexcept { return data_; }

    /*!
     * \brief Get the size of the data.
     *
     * \return Size of the data.
     */
    [[nodiscard]] shm_stream_size_t size() const noexcept { return size_; }

    /*!
     * \brief Check whether this buffer is empty.
     *
     * \retval true This buffer is empty.
     * \retval false This buffer is not empty.
     */
    [[nodiscard]] bool empty() const noexcept { return size_ == 0U; }

private:
    //! Pointer to the data.
    char* data_;

    //! Size of the data.
    shm_stream_size_t size_;
};

/*!
 * \brief Class of views of constant byte sequences.
 */
class bytes_view {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] data Pointer to the data.
     * \param[in] size Size of the data.
     */
    constexpr bytes_view(const char* data, shm_stream_size_t size) noexcept
        : data_(data), size_(size) {}

    /*!
     * \brief Get the pointer to the data.
     *
     * \return Pointer to the data.
     */
    [[nodiscard]] constexpr const char* data() const noexcept { return data_; }

    /*!
     * \brief Get the size of the data.
     *
     * \return Size of the data.
     */
    [[nodiscard]] constexpr shm_stream_size_t size() const noexcept {
        return size_;
    }

    /*!
     * \brief Check whether this buffer is empty.
     *
     * \retval true This buffer is empty.
     * \retval false This buffer is not empty.
     */
    [[nodiscard]] bool empty() const noexcept { return size_ == 0U; }

private:
    //! Pointer to the data.
    const char* data_;

    //! Size of the data.
    shm_stream_size_t size_;
};

}  // namespace shm_stream
