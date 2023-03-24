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
 * \brief Definition of light streams of bytes without waiting (possibly
 * lock-free and wait-free).
 */
#pragma once

#include "shm_stream/bytes_view.h"
#include "shm_stream/c_interface/light_stream_common.h"
#include "shm_stream/c_interface/light_stream_writer.h"
#include "shm_stream/c_interface/string_view.h"
#include "shm_stream/common_types.h"
#include "shm_stream/details/shm_stream_export.h"
#include "shm_stream/details/smart_ptr.h"
#include "shm_stream/details/throw_if_error.h"
#include "shm_stream/string_view.h"

namespace shm_stream {

/*!
 * \brief Class of writer of light streams of bytes without waiting (possibly
 * lock-free and wait-free).
 */
class light_stream_writer {
public:
    /*!
     * \brief Constructor.
     */
    light_stream_writer() = default;

    // Prevent copy.
    light_stream_writer(const light_stream_writer&) = delete;
    auto operator=(const light_stream_writer&) = delete;

    /*!
     * \brief Move constructor.
     */
    light_stream_writer(light_stream_writer&& /*obj*/) noexcept = default;

    /*!
     * \brief Move assignment operator.
     *
     * \return This.
     */
    light_stream_writer& operator=(
        light_stream_writer&& /*obj*/) noexcept = default;

    /*!
     * \brief Destructor.
     *
     * \note This function will automatically close this stream.
     */
    ~light_stream_writer() noexcept = default;

    /*!
     * \brief Open a stream.
     *
     * \param[in] name Name of the stream.
     * \param[in] buffer_size Size of the buffer.
     */
    void open(string_view name, shm_stream_size_t buffer_size) {
        c_shm_stream_light_stream_writer_t* writer{nullptr};
        details::throw_if_error(c_shm_stream_light_stream_writer_create(&writer,
            c_shm_stream_string_view_t{name.data(), name.size()}, buffer_size));
        writer_ = details::smart_ptr<c_shm_stream_light_stream_writer_t>(
            writer, c_shm_stream_light_stream_writer_destroy);
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
     * \brief Get the size of the available bytes to write.
     *
     * \return Size of the available bytes to write.
     */
    [[nodiscard]] shm_stream_size_t available_size() const noexcept {
        return c_shm_stream_light_stream_writer_available_size(writer_.get());
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
     */
    [[nodiscard]] mutable_bytes_view try_reserve(
        shm_stream_size_t expected_size) noexcept {
        const auto buf = c_shm_stream_light_stream_writer_try_reserve(
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
     */
    [[nodiscard]] mutable_bytes_view try_reserve() noexcept {
        const auto buf =
            c_shm_stream_light_stream_writer_try_reserve_all(writer_.get());
        return mutable_bytes_view(buf.data, buf.size);
    }

    /*!
     * \brief Save written bytes as completed and ready to be read by a reader.
     *
     * \param[in] written_size Number of written bytes to save.
     */
    void commit(shm_stream_size_t written_size) noexcept {
        c_shm_stream_light_stream_writer_commit(writer_.get(), written_size);
    }

private:
    //! Actual writer in C interface.
    details::smart_ptr<c_shm_stream_light_stream_writer_t> writer_{};
};

/*!
 * \brief Class of reader of light streams of bytes without waiting (possibly
 * lock-free and wait-free).
 */
class SHM_STREAM_EXPORT light_stream_reader {
public:
    /*!
     * \brief Constructor.
     */
    light_stream_reader();

    // Prevent copy.
    light_stream_reader(const light_stream_reader&) = delete;
    auto operator=(const light_stream_reader&) = delete;

    /*!
     * \brief Move constructor.
     *
     * \param[in] obj Object to move from.
     */
    light_stream_reader(light_stream_reader&& obj) noexcept;

    /*!
     * \brief Move assignment operator.
     *
     * \param[in] obj Object to move from.
     * \return This.
     */
    light_stream_reader& operator=(light_stream_reader&& obj) noexcept;

    /*!
     * \brief Destructor.
     *
     * \note This function will automatically close this stream.
     */
    ~light_stream_reader() noexcept;

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
 * \brief Classes and functions of light streams of bytes without waiting
 * (possibly lock-free and wait-free).
 */
namespace light_stream {

/*!
 * \brief Class of writer of streams of bytes without waiting (possibly
 * lock-free and wait-free).
 */
using writer = light_stream_writer;

/*!
 * \brief Class of reader of streams of bytes without waiting (possibly
 * lock-free and wait-free).
 */
using reader = light_stream_reader;

/*!
 * \brief Create a stream.
 *
 * \param[in] name Name of the stream.
 * \param[in] buffer_size Size of the buffer.
 */
void create(string_view name, shm_stream_size_t buffer_size) {
    details::throw_if_error(c_shm_stream_light_stream_create(
        c_shm_stream_string_view_t{name.data(), name.size()}, buffer_size));
}

/*!
 * \brief Remove a stream.
 *
 * \param[in] name Name of the stream.
 */
void remove(string_view name) {
    c_shm_stream_light_stream_remove(
        c_shm_stream_string_view_t{name.data(), name.size()});
}

}  // namespace light_stream

}  // namespace shm_stream
