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
 * \brief Definition of smart_ptr class.
 */
#pragma once

#include <utility>

namespace shm_stream {
namespace details {

/*!
 * \brief Class of smart pointers of objects in C interfaces.
 *
 * \tparam T Type of objects pointed by this class.
 */
template <typename T>
class smart_ptr {
public:
    //! Type of deleter functions.
    using deleter_type = void (*)(T*);

    /*!
     * \brief Constructor. (Initialize to a null pointer.)
     */
    smart_ptr() noexcept : ptr_(nullptr), deleter_(nullptr) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] ptr Pointer.
     * \param[in] deleter Deleter.
     */
    smart_ptr(T* ptr, deleter_type deleter) noexcept
        : ptr_(ptr), deleter_(deleter) {}

    smart_ptr(const smart_ptr&) = delete;
    smart_ptr& operator=(const smart_ptr&) = delete;

    /*!
     * \brief Move constructor.
     *
     * \param[in] obj Object to move from.
     */
    smart_ptr(smart_ptr&& obj) noexcept
        : ptr_(std::exchange(obj.ptr_, nullptr)), deleter_(obj.deleter_) {}

    /*!
     * \brief Move assignment operator.
     *
     * \param[in] obj Object to move from.
     * \return This.
     */
    smart_ptr& operator=(smart_ptr&& obj) noexcept {
        std::swap(ptr_, obj.ptr_);
        std::swap(deleter_, obj.deleter_);
        return *this;
    }

    /*!
     * \brief Destructor.
     */
    ~smart_ptr() noexcept { reset(); }

    /*!
     * \brief Destroy the object pointed by this pointer.
     */
    void reset() noexcept {
        if (ptr_ == nullptr) {
            return;
        }
        deleter_(ptr_);
        ptr_ = nullptr;
    }

    /*!
     * \brief Get the pointer.
     *
     * \return Pointer.
     */
    [[nodiscard]] T* get() const noexcept { return ptr_; }

    /*!
     * \brief Check whether this pointer has an object.
     *
     * \retval true This pointer has an object.
     * \retval false This pointer doesn't has an object. (This pointer is a null
     * pointer.)
     */
    [[nodiscard]] bool has_obj() const noexcept { return ptr_ != nullptr; }

private:
    //! Pointer.
    T* ptr_;

    //! Deleter.
    deleter_type deleter_;
};

}  // namespace details
}  // namespace shm_stream
