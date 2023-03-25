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
 * \brief Definition of C interface of blocking streams of bytes with wait
 * operations.
 */
#pragma once

#include "shm_stream/c_interface/bytes_view.h"
#include "shm_stream/c_interface/common_types.h"
#include "shm_stream/c_interface/error_codes.h"
#include "shm_stream/c_interface/string_view.h"
#include "shm_stream/details/shm_stream_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Reader of blocking streams of bytes with wait operations.
 */
struct c_shm_stream_blocking_stream_reader;

/*!
 * \brief Reader of blocking streams of bytes with wait operations.
 */
typedef struct c_shm_stream_blocking_stream_reader
    c_shm_stream_blocking_stream_reader_t;

/*!
 * \brief Create a reader of a blocking stream.
 *
 * \param[out] reader Reader.
 * \param[in] name Name of the stream.
 * \param[in] buffer_size Size of the buffer.
 * \return Error code.
 */
SHM_STREAM_EXPORT c_shm_stream_error_code_t
c_shm_stream_blocking_stream_reader_create(
    c_shm_stream_blocking_stream_reader_t** reader,
    c_shm_stream_string_view_t name, c_shm_stream_size_t buffer_size);

/*!
 * \brief Destroy a reader of a blocking stream.
 *
 * \param[in] reader Reader.
 */
SHM_STREAM_EXPORT void c_shm_stream_blocking_stream_reader_destroy(
    c_shm_stream_blocking_stream_reader_t* reader);

/*!
 * \brief Get the number of the available bytes to read.
 *
 * \param[in] reader Reader.
 * \return Number of the available bytes to read.
 *
 * \note After stop of this stream, this function returns zero.
 */
SHM_STREAM_EXPORT c_shm_stream_size_t
c_shm_stream_blocking_stream_reader_available_size(
    c_shm_stream_blocking_stream_reader_t* reader);

/*!
 * \brief Wait until some bytes are available.
 *
 * \param[in] reader Reader.
 * \return Number of the available bytes to read.
 *
 * \note After stop of this stream, this function immediately returns zero.
 */
SHM_STREAM_EXPORT c_shm_stream_size_t c_shm_stream_blocking_stream_reader_wait(
    c_shm_stream_blocking_stream_reader_t* reader);

/*!
 * \brief Stop this stream.
 *
 * \param[in] reader Reader.
 */
SHM_STREAM_EXPORT void c_shm_stream_blocking_stream_reader_stop(
    c_shm_stream_blocking_stream_reader_t* reader);

/*!
 * \brief Check whether this stream is stopped.
 *
 * \param[in] reader Reader.
 * \retval true This stream is stopped.
 * \retval false This stream is not stopped.
 */
SHM_STREAM_EXPORT bool c_shm_stream_blocking_stream_reader_is_stopped(
    c_shm_stream_blocking_stream_reader_t* reader);

/*!
 * \brief Try to reserve some bytes to read.
 *
 * \param[in] reader Reader.
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
SHM_STREAM_EXPORT c_shm_stream_bytes_view_t
c_shm_stream_blocking_stream_reader_try_reserve(
    c_shm_stream_blocking_stream_reader_t* reader,
    c_shm_stream_size_t expected_size);

/*!
 * \brief Try to reserve some bytes to read as many as possible.
 *
 * \param[in] reader Reader.
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
SHM_STREAM_EXPORT c_shm_stream_bytes_view_t
c_shm_stream_blocking_stream_reader_try_reserve_all(
    c_shm_stream_blocking_stream_reader_t* reader);

/*!
 * \brief Wait to reserve some bytes to read.
 *
 * \param[in] reader Reader.
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
SHM_STREAM_EXPORT c_shm_stream_bytes_view_t
c_shm_stream_blocking_stream_reader_wait_reserve(
    c_shm_stream_blocking_stream_reader_t* reader,
    c_shm_stream_size_t expected_size);

/*!
 * \brief Wait to reserve some bytes to read as many as possible.
 *
 * \param[in] reader Reader.
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
SHM_STREAM_EXPORT c_shm_stream_bytes_view_t
c_shm_stream_blocking_stream_reader_wait_reserve_all(
    c_shm_stream_blocking_stream_reader_t* reader);

/*!
 * \brief Set some bytes as finished to read and ready to be written by a
 * writer.
 *
 * \param[in] reader Reader.
 * \param[in] read_size Number of read bytes to save.
 */
SHM_STREAM_EXPORT void c_shm_stream_blocking_stream_reader_commit(
    c_shm_stream_blocking_stream_reader_t* reader,
    c_shm_stream_size_t read_size);

#ifdef __cplusplus
}
#endif
