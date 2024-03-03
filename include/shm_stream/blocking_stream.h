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
 * \brief Definition of blocking streams of bytes with wait operations.
 */
#pragma once

#include "shm_stream/bytes_view.h"
#include "shm_stream/c_interface/blocking_stream_common.h"
#include "shm_stream/c_interface/blocking_stream_reader.h"
#include "shm_stream/c_interface/blocking_stream_writer.h"
#include "shm_stream/c_interface/string_view.h"
#include "shm_stream/common_types.h"
#include "shm_stream/details/smart_ptr.h"
#include "shm_stream/details/throw_if_error.h"
#include "shm_stream/string_view.h"

namespace shm_stream {

/*!
 * \brief Class of writer of blocking streams of bytes with wait operations.
 *
 * \thread_safety All operation is safe if only one writer exists,
 * except for stop and is_stopped functions which are safe to call from any
 * threads.
 */
class blocking_stream_writer {
public:
    /*!
     * \brief Constructor.
     */
    blocking_stream_writer() = default;

    // Prevent copy.
    blocking_stream_writer(const blocking_stream_writer&) = delete;
    auto operator=(const blocking_stream_writer&) = delete;

    /*!
     * \brief Move constructor.
     */
    blocking_stream_writer(blocking_stream_writer&& /*obj*/) noexcept = default;

    /*!
     * \brief Move assignment operator.
     *
     * \return This.
     */
    blocking_stream_writer& operator=(
        blocking_stream_writer&& /*obj*/) noexcept = default;

    /*!
     * \brief Destructor.
     *
     * \note This function will automatically close this stream.
     */
    ~blocking_stream_writer() noexcept = default;

    /*!
     * \brief Open a stream.
     *
     * \param[in] name Name of the stream.
     * \param[in] buffer_size Size of the buffer.
     */
    void open(string_view name, shm_stream_size_t buffer_size) {
        c_shm_stream_blocking_stream_writer_t* writer{nullptr};
        details::throw_if_error(c_shm_stream_blocking_stream_writer_create(
            &writer, c_shm_stream_string_view_t{name.data(), name.size()},
            buffer_size));
        writer_ = details::smart_ptr<c_shm_stream_blocking_stream_writer_t>(
            writer, c_shm_stream_blocking_stream_writer_destroy);
    }

    /*!
     * \brief Close a stream.
     *
     * \note This function can be called when this stream has been already
     * closed.
     */
    void close() noexcept { writer_.reset(); }

    /*!
     * \brief Check whether this object is opened.
     *
     * \retval true This object is opened.
     * \retval false This object is not opened.
     */
    [[nodiscard]] bool is_opened() const noexcept { return writer_.has_obj(); }

    /*!
     * \brief Get the number of the available bytes to write.
     *
     * \return Number of the available bytes to write.
     *
     * \note After stop of this stream, this function returns zero.
     */
    [[nodiscard]] shm_stream_size_t available_size() const noexcept {
        return c_shm_stream_blocking_stream_writer_available_size(
            writer_.get());
    }

    /*!
     * \brief Wait until some bytes are available.
     *
     * \return Number of the available bytes to write.
     *
     * \note After stop of this stream, this function immediately returns zero.
     */
    shm_stream_size_t wait() const noexcept {
        return c_shm_stream_blocking_stream_writer_wait(writer_.get());
    }

    /*!
     * \brief Stop this stream.
     */
    void stop() noexcept {
        c_shm_stream_blocking_stream_writer_stop(writer_.get());
    }

    /*!
     * \brief Check whether this stream is stopped.
     *
     * \retval true This stream is stopped.
     * \retval false This stream is not stopped.
     */
    [[nodiscard]] bool is_stopped() const noexcept {
        return c_shm_stream_blocking_stream_writer_is_stopped(writer_.get());
    }

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
     * \note After stop of this stream, this function returns empty buffers.
     */
    [[nodiscard]] mutable_bytes_view try_reserve(
        shm_stream_size_t expected_size) noexcept {
        const auto buf = c_shm_stream_blocking_stream_writer_try_reserve(
            writer_.get(), expected_size);
        return mutable_bytes_view(buf.data, buf.size);
    }

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
     * \note After stop of this stream, this function returns empty buffers.
     */
    [[nodiscard]] mutable_bytes_view try_reserve() noexcept {
        const auto buf =
            c_shm_stream_blocking_stream_writer_try_reserve_all(writer_.get());
        return mutable_bytes_view(buf.data, buf.size);
    }

    /*!
     * \brief Wait to reserve some bytes to write.
     *
     * \param[in] expected_size Expected number of bytes to reserve to write.
     * \return Buffer of the reserved bytes.
     *
     * \note This function returns when at least one byte is available.
     * \note This function can return a buffer with a size smaller than the
     * return value of available_size function, because this stream uses a
     * circular buffer in the implementation and this function reserves
     * continuous byte sequences from the circular buffer.
     * \note After stop of this stream, this function immediately returns empty
     * buffers.
     */
    [[nodiscard]] mutable_bytes_view wait_reserve(
        shm_stream_size_t expected_size) noexcept {
        const auto buf = c_shm_stream_blocking_stream_writer_wait_reserve(
            writer_.get(), expected_size);
        return mutable_bytes_view(buf.data, buf.size);
    }

    /*!
     * \brief Wait to reserve some bytes to read as many as possible.
     *
     * \return Buffer of the reserved bytes.
     *
     * \note This function returns when at least one byte is available.
     * \note This function can return a buffer with a size smaller than the
     * return value of available_size function, because this stream uses a
     * circular buffer in the implementation and this function reserves
     * continuous byte sequences from the circular buffer.
     * \note After stop of this stream, this function immediately returns empty
     * buffers.
     */
    [[nodiscard]] mutable_bytes_view wait_reserve() noexcept {
        const auto buf =
            c_shm_stream_blocking_stream_writer_wait_reserve_all(writer_.get());
        return mutable_bytes_view(buf.data, buf.size);
    }

    /*!
     * \brief Save written bytes as completed and ready to be read by a reader.
     *
     * \param[in] written_size Number of written bytes to save.
     */
    void commit(shm_stream_size_t written_size) noexcept {
        c_shm_stream_blocking_stream_writer_commit(writer_.get(), written_size);
    }

private:
    //! Actual writer in C interface.
    details::smart_ptr<c_shm_stream_blocking_stream_writer_t> writer_{};
};

/*!
 * \brief Class of reader of blocking streams of bytes with wait operations.
 *
 * \thread_safety All operation is safe if only one reader exists,
 * except for stop and is_stopped functions which are safe to call from any
 * threads.
 */
class blocking_stream_reader {
public:
    /*!
     * \brief Constructor.
     */
    blocking_stream_reader() = default;

    // Prevent copy.
    blocking_stream_reader(const blocking_stream_reader&) = delete;
    auto operator=(const blocking_stream_reader&) = delete;

    /*!
     * \brief Move constructor.
     *
     * \param[in] obj Object to move from.
     */
    blocking_stream_reader(blocking_stream_reader&& obj) noexcept = default;

    /*!
     * \brief Move assignment operator.
     *
     * \param[in] obj Object to move from.
     * \return This.
     */
    blocking_stream_reader& operator=(
        blocking_stream_reader&& obj) noexcept = default;

    /*!
     * \brief Destructor.
     *
     * \note This function will automatically close this stream.
     */
    ~blocking_stream_reader() noexcept = default;

    /*!
     * \brief Open a stream.
     *
     * \param[in] name Name of the stream.
     * \param[in] buffer_size Size of the buffer.
     */
    void open(string_view name, shm_stream_size_t buffer_size) {
        c_shm_stream_blocking_stream_reader_t* reader{nullptr};
        details::throw_if_error(c_shm_stream_blocking_stream_reader_create(
            &reader, c_shm_stream_string_view_t{name.data(), name.size()},
            buffer_size));
        reader_ = details::smart_ptr<c_shm_stream_blocking_stream_reader_t>(
            reader, c_shm_stream_blocking_stream_reader_destroy);
    }

    /*!
     * \brief Close a stream.
     *
     * \note This function can be called when this stream has been already
     * closed.
     */
    void close() noexcept { reader_.reset(); }

    /*!
     * \brief Check whether this object is opened.
     *
     * \retval true This object is opened.
     * \retval false This object is not opened.
     */
    [[nodiscard]] bool is_opened() const noexcept { return reader_.has_obj(); }

    /*!
     * \brief Get the number of the available bytes to read.
     *
     * \return Number of the available bytes to read.
     *
     * \note After stop of this stream, this function returns zero.
     */
    [[nodiscard]] shm_stream_size_t available_size() const noexcept {
        return c_shm_stream_blocking_stream_reader_available_size(
            reader_.get());
    }

    /*!
     * \brief Get the number of the available bytes to read.
     *
     * \return Number of the available bytes to read.
     *
     * \note After stop of this stream, this function immediately returns zero.
     */
    shm_stream_size_t wait() const noexcept {
        return c_shm_stream_blocking_stream_reader_wait(reader_.get());
    }

    /*!
     * \brief Stop this stream.
     */
    void stop() noexcept {
        c_shm_stream_blocking_stream_reader_stop(reader_.get());
    }

    /*!
     * \brief Check whether this stream is stopped.
     *
     * \retval true This stream is stopped.
     * \retval false This stream is not stopped.
     */
    [[nodiscard]] bool is_stopped() const noexcept {
        return c_shm_stream_blocking_stream_reader_is_stopped(reader_.get());
    }

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
     * \note After stop of this stream, this function returns empty buffers.
     */
    [[nodiscard]] bytes_view try_reserve(
        shm_stream_size_t expected_size) noexcept {
        const auto buf = c_shm_stream_blocking_stream_reader_try_reserve(
            reader_.get(), expected_size);
        return bytes_view(buf.data, buf.size);
    }

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
     * \note After stop of this stream, this function returns empty buffers.
     */
    [[nodiscard]] bytes_view try_reserve() noexcept {
        const auto buf =
            c_shm_stream_blocking_stream_reader_try_reserve_all(reader_.get());
        return bytes_view(buf.data, buf.size);
    }

    /*!
     * \brief Wait to reserve some bytes to read.
     *
     * \param[in] expected_size Expected number of bytes to reserve to read.
     * \return Buffer of the reserved bytes.
     *
     * \note This function returns when at least one byte is available.
     * \note This function can return a buffer with a size smaller than the
     * return value of available_size function, because this stream uses a
     * circular buffer in the implementation and this function reserves
     * continuous byte sequences from the circular buffer.
     * \note After stop of this stream, this function immediately returns empty
     * buffers.
     */
    [[nodiscard]] bytes_view wait_reserve(
        shm_stream_size_t expected_size) noexcept {
        const auto buf = c_shm_stream_blocking_stream_reader_wait_reserve(
            reader_.get(), expected_size);
        return bytes_view(buf.data, buf.size);
    }

    /*!
     * \brief Wait to reserve some bytes to read as many as possible.
     *
     * \return Buffer of the reserved bytes.
     *
     * \note This function returns when at least one byte is available.
     * \note This function can return a buffer with a size smaller than the
     * return value of available_size function, because this stream uses a
     * circular buffer in the implementation and this function reserves
     * continuous byte sequences from the circular buffer.
     * \note After stop of this stream, this function immediately returns empty
     * buffers.
     */
    [[nodiscard]] bytes_view wait_reserve() noexcept {
        const auto buf =
            c_shm_stream_blocking_stream_reader_wait_reserve_all(reader_.get());
        return bytes_view(buf.data, buf.size);
    }

    /*!
     * \brief Set some bytes as finished to read and ready to be written by a
     * writer.
     *
     * \param[in] read_size Number of read bytes to save.
     */
    void commit(shm_stream_size_t read_size) noexcept {
        c_shm_stream_blocking_stream_reader_commit(reader_.get(), read_size);
    }

private:
    //! Actual reader in C interface.
    details::smart_ptr<c_shm_stream_blocking_stream_reader_t> reader_{};
};

/*!
 * \brief Classes and functions of blocking streams of bytes with wait
 * operations.
 */
namespace blocking_stream {

/*!
 * \brief Class of writer of blocking streams of bytes with wait operations.
 */
using writer = blocking_stream_writer;

/*!
 * \brief Class of reader of blocking streams of bytes with wait operations.
 */
using reader = blocking_stream_reader;

/*!
 * \brief Create a stream.
 *
 * \param[in] name Name of the stream.
 * \param[in] buffer_size Size of the buffer.
 */
inline void create(string_view name, shm_stream_size_t buffer_size) {
    details::throw_if_error(c_shm_stream_blocking_stream_create(
        c_shm_stream_string_view_t{name.data(), name.size()}, buffer_size));
}

/*!
 * \brief Remove a stream.
 *
 * \param[in] name Name of the stream.
 */
inline void remove(string_view name) {
    c_shm_stream_blocking_stream_remove(
        c_shm_stream_string_view_t{name.data(), name.size()});
}

}  // namespace blocking_stream

}  // namespace shm_stream
