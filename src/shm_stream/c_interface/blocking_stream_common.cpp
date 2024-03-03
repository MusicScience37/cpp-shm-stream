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
 * \brief Implementation of C interface of blocking streams of bytes with wait
 * operations.
 */
#include "shm_stream/c_interface/blocking_stream_common.h"

#include "blocking_stream_internal.h"
#include "shm_stream/c_interface/common_types.h"
#include "shm_stream/c_interface/error_codes.h"
#include "shm_stream/c_interface/string_view.h"
#include "shm_stream/c_interface/translate_error.h"
#include "shm_stream/string_view.h"

c_shm_stream_error_code_t c_shm_stream_blocking_stream_create(
    c_shm_stream_string_view_t name, c_shm_stream_size_t buffer_size) {
    C_SHM_STREAM_TRANSLATE_ERROR(
        (void)shm_stream::details::prepare_blocking_stream_data(
            shm_stream::string_view(name.data, name.size), buffer_size));
}

void c_shm_stream_blocking_stream_remove(c_shm_stream_string_view_t name) {
    C_SHM_STREAM_NO_ERROR(shm_stream::details::remove_blocking_stream(
        shm_stream::string_view(name.data, name.size)));
}
