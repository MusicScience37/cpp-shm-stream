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
 * \brief Definition of streams of bytes without waiting (possibly lock-free).
 */
#pragma once

#include "shm_stream/bytes_view.h"
#include "shm_stream/common_types.h"
#include "shm_stream/details/shm_stream_export.h"
#include "shm_stream/string_view.h"

namespace shm_stream {

/*!
 * \brief Class of writer of streams of bytes without waiting (possibly
 * lock-free).
 */
class SHM_STREAM_EXPORT no_wait_stream_writer {
public:
    /*!
     * \brief Constructor.
     */
    no_wait_stream_writer();

    // Prevent copy.
    no_wait_stream_writer(const no_wait_stream_writer&) = delete;
    auto operator=(const no_wait_stream_writer&) = delete;

    // TODO Implement move.
    no_wait_stream_writer(no_wait_stream_writer&&) = delete;
    auto operator=(no_wait_stream_writer&&) = delete;

    /*!
     * \brief Destructor.
     *
     * \note This function will automatically close this stream.
     */
    ~no_wait_stream_writer() noexcept;

    /*!
     * \brief Open a stream.
     *
     * \param[in] name Name of the stream.
     * \param[in] buffer_size Size of the buffer.
     */
    void open(string_view name, shm_stream_size_t buffer_size);

    /*!
     * \brief Close a stream.
     *
     * \note This function can be called when this stream has been already
     * closed.
     */
    void close() noexcept;

    /*!
     * \brief Get the size of the available bytes to write.
     *
     * \return Size of the available bytes to write.
     */
    [[nodiscard]] shm_stream_size_t available_size() const noexcept;

    /*!
     * \brief Try to reserve some bytes to write.
     *
     * \param[in] expected_size Expected number of bytes to reserve to write.
     * \return Buffer of the reserved bytes.
     *
     * \note This function tries to reserve given number of bytes, but a smaller
     * or empty buffer may be returned.
     * \note This function can return a buffer with a size smaller than the
     * return value of available_size function, because this queue is a circular
     * buffer and this function reserves continuous byte sequences from the
     * circular buffer.
     */
    [[nodiscard]] mutable_bytes_view try_reserve(
        shm_stream_size_t expected_size) noexcept;

    /*!
     * \brief Try to reserve some bytes to write as many as possible.
     *
     * \return Buffer of the reserved bytes.
     *
     * \note This function tries to reserve given number of bytes, but a smaller
     * or empty buffer may be returned.
     * \note This function can return a buffer with a size smaller than the
     * return value of available_size function, because this queue is a circular
     * buffer and this function reserves continuous byte sequences from the
     * circular buffer.
     */
    [[nodiscard]] mutable_bytes_view try_reserve() noexcept;

    /*!
     * \brief Save written bytes as completed and ready to be read by a reader.
     *
     * \param[in] written_size Number of written bytes to save.
     */
    void commit(shm_stream_size_t written_size) noexcept;

private:
    //! Type of the internal data.
    struct impl_type;

    //! Internal data.
    impl_type* impl_;
};

}  // namespace shm_stream
