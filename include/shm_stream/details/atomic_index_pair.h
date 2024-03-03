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
 * \brief Definition of atomic_index_pair class.
 */
#pragma once

#include <type_traits>

#include <boost/atomic/ipc_atomic.hpp>

#include "shm_stream/common_types.h"
#include "shm_stream/details/cache_line_size.h"
#include "shm_stream/shm_stream_assert.h"

namespace shm_stream {
namespace details {

/*!
 * \brief Class of pairs of atomic variables for indices of readers and writers.
 *
 * \tparam AtomicType Type of atomic variables.
 */
template <typename AtomicType = boost::atomics::ipc_atomic<shm_stream_size_t>>
class atomic_index_pair {
public:
    //! Type of the atomic variables.
    using atomic_type = AtomicType;

    static_assert(std::is_same<typename atomic_type::value_type,
                      shm_stream_size_t>::value,
        "Type of values in atomic variables must be equal to "
        "shm_stream_size_t.");

    /*!
     * \brief Constructor.
     */
    atomic_index_pair() = default;

    /*!
     * \brief Get the index of the writer.
     *
     * \return Atomic variable of the index.
     */
    [[nodiscard]] atomic_type& writer() noexcept { return writer_index_; }

    /*!
     * \brief Get the index of the reader.
     *
     * \return Atomic variable of the index.
     */
    [[nodiscard]] atomic_type& reader() noexcept { return reader_index_; }

private:
    //! Index of the writer.
    alignas(cache_line_size()) atomic_type writer_index_{0U};

    //! Index of the reader.
    alignas(cache_line_size()) atomic_type reader_index_{0U};
};

/*!
 * \brief Class of views of pairs of atomic variables for indices of readers and
 * writers.
 *
 * \tparam AtomicType Type of atomic variables.
 */
template <typename AtomicType = boost::atomics::ipc_atomic<shm_stream_size_t>>
class atomic_index_pair_view {
public:
    //! Type of the atomic variables.
    using atomic_type = AtomicType;

    static_assert(std::is_same<typename atomic_type::value_type,
                      shm_stream_size_t>::value,
        "Type of values in atomic variables must be equal to "
        "shm_stream_size_t.");

    /*!
     * \brief Constructor.
     *
     * \param[in] writer_index Index of the writer.
     * \param[in] reader_index Index of the reader.
     */
    atomic_index_pair_view(atomic_type* writer_index, atomic_type* reader_index)
        : writer_index_(writer_index), reader_index_(reader_index) {
        SHM_STREAM_ASSERT(writer_index_ != nullptr);
        SHM_STREAM_ASSERT(reader_index_ != nullptr);
    }

    /*!
     * \brief Constructor. (Implicit conversion from atomic_index_pair.)
     *
     * \param[in] indices Indices.
     */
    atomic_index_pair_view(  // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
        atomic_index_pair<atomic_type>& indices)
        : atomic_index_pair_view(&indices.writer(), &indices.reader()) {}

    /*!
     * \brief Get the index of the writer.
     *
     * \return Atomic variable of the index.
     */
    [[nodiscard]] atomic_type& writer() noexcept { return *writer_index_; }

    /*!
     * \brief Get the index of the reader.
     *
     * \return Atomic variable of the index.
     */
    [[nodiscard]] atomic_type& reader() noexcept { return *reader_index_; }

private:
    //! Index of the writer.
    atomic_type* writer_index_;

    //! Index of the reader.
    atomic_type* reader_index_;
};

}  // namespace details
}  // namespace shm_stream
