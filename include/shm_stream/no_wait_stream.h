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
 * \brief Definition of streams of bytes without waiting (possibly lock-free and
 * wait-free).
 */
#pragma once

#include "shm_stream/bytes_view.h"
#include "shm_stream/common_types.h"
#include "shm_stream/details/shm_stream_export.h"
#include "shm_stream/string_view.h"

namespace shm_stream {

/*!
 * \brief Class of writer of streams of bytes without waiting (possibly
 * lock-free and wait-free).
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

    /*!
     * \brief Move constructor.
     *
     * \param[in] obj Object to move from.
     */
    no_wait_stream_writer(no_wait_stream_writer&& obj) noexcept;

    /*!
     * \brief Move assignment operator.
     *
     * \param[in] obj Object to move from.
     * \return This.
     */
    no_wait_stream_writer& operator=(no_wait_stream_writer&& obj) noexcept;

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
     * \brief Check whether this object is opened.
     *
     * \retval true This object is opened.
     * \retval false This object is not opened.
     */
    [[nodiscard]] bool is_opened() const noexcept;

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
     * return value of available_size function, because this stream uses a
     * circular buffer in the implementation and this function reserves
     * continuous byte sequences from the circular buffer.
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
     * return value of available_size function, because this stream uses a
     * circular buffer in the implementation and this function reserves
     * continuous byte sequences from the circular buffer.
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

/*!
 * \brief Class of reader of streams of bytes without waiting (possibly
 * lock-free and wait-free).
 */
class SHM_STREAM_EXPORT no_wait_stream_reader {
public:
    /*!
     * \brief Constructor.
     */
    no_wait_stream_reader();

    // Prevent copy.
    no_wait_stream_reader(const no_wait_stream_reader&) = delete;
    auto operator=(const no_wait_stream_reader&) = delete;

    /*!
     * \brief Move constructor.
     *
     * \param[in] obj Object to move from.
     */
    no_wait_stream_reader(no_wait_stream_reader&& obj) noexcept;

    /*!
     * \brief Move assignment operator.
     *
     * \param[in] obj Object to move from.
     * \return This.
     */
    no_wait_stream_reader& operator=(no_wait_stream_reader&& obj) noexcept;

    /*!
     * \brief Destructor.
     *
     * \note This function will automatically close this stream.
     */
    ~no_wait_stream_reader() noexcept;

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
     * \brief Check whether this object is opened.
     *
     * \retval true This object is opened.
     * \retval false This object is not opened.
     */
    [[nodiscard]] bool is_opened() const noexcept;

    /*!
     * \brief Get the size of the available bytes to read.
     *
     * \return Size of the available bytes to read.
     */
    [[nodiscard]] shm_stream_size_t available_size() const noexcept;

    /*!
     * \brief Try to reserve some bytes to read.
     *
     * \param[in] expected_size Expected number of bytes to reserve to read.
     * \return Buffer of the reserved bytes.
     *
     * \note This function tries to reserve given number of bytes, but a smaller
     * or empty buffer may be returned.
     * \note This function can return a buffer with a size smaller than the
     * return value of available_size function, because this stream uses a
     * circular buffer in the implementation and this function reserves
     * continuous byte sequences from the circular buffer.
     */
    [[nodiscard]] bytes_view try_reserve(
        shm_stream_size_t expected_size) noexcept;

    /*!
     * \brief Try to reserve some bytes to read as many as possible.
     *
     * \return Buffer of the reserved bytes.
     *
     * \note This function tries to reserve given number of bytes, but a smaller
     * or empty buffer may be returned.
     * \note This function can return a buffer with a size smaller than the
     * return value of available_size function, because this stream uses a
     * circular buffer in the implementation and this function reserves
     * continuous byte sequences from the circular buffer.
     */
    [[nodiscard]] bytes_view try_reserve() noexcept;

    /*!
     * \brief Set some bytes as finished to read and ready to be written by a
     * writer.
     *
     * \param[in] read_size Number of read bytes to save.
     */
    void commit(shm_stream_size_t read_size) noexcept;

private:
    //! Type of the internal data.
    struct impl_type;

    //! Internal data.
    impl_type* impl_;
};

/*!
 * \brief Classes and functions of streams of bytes without waiting (possibly
 * lock-free and wait-free).
 */
namespace no_wait_stream {

/*!
 * \brief Class of writer of streams of bytes without waiting (possibly
 * lock-free and wait-free).
 */
using writer = no_wait_stream_writer;

/*!
 * \brief Class of reader of streams of bytes without waiting (possibly
 * lock-free and wait-free).
 */
using reader = no_wait_stream_reader;

/*!
 * \brief Create a stream.
 *
 * \param[in] name Name of the stream.
 * \param[in] buffer_size Size of the buffer.
 */
SHM_STREAM_EXPORT void create(string_view name, shm_stream_size_t buffer_size);

/*!
 * \brief Remove a stream.
 *
 * \param[in] name Name of the stream.
 */
SHM_STREAM_EXPORT void remove(string_view name);

}  // namespace no_wait_stream

}  // namespace shm_stream
