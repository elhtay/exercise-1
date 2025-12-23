/*
 * SPDX-FileCopyrightText: Copyright 2023 Arm Limited and/or its affiliates
 * <open-source-office@arm.com> SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <cstddef>
#include <new>
#include <utility>
/**
 * @brief A memory pool for objects of type T with a fixed size N
 *
 * @tparam T type of objects to be stored in the pool
 * @tparam N maximum number of objects in the pool
 */
template <typename T, std::size_t N>
class Pool
{
public:
  Pool() : nextFree(0)
  {
    // initialize free list, each index points to the next free slot
    for (std::size_t i = 0; i < N - 1; ++i)
    {
      freeIndices[i] = i + 1;
    }
    freeIndices[N - 1] = INVALID_INDEX;
  }

  /** @brief Destroy the pool and free all allocated objects */
  virtual ~Pool() {}

  /** @brief Create an object of type T in the pool
   *
   * @tparam Args Types of the constructor arguments
   * @param args Constructor arguments
   * @return T* Pointer to the created object, or nullptr if the pool is full
   */
  template <typename... Args>
  T *makeObject(Args &&...args) // void in the original template but T* in unit tests
  {
    if (nextFree == INVALID_INDEX)
    {
      return nullptr;
    }

    // get next free index
    std::size_t currentIndex = nextFree;
    nextFree = freeIndices[currentIndex];

    // get pointer to storage location
    T *pointer = reinterpret_cast<T *>(&storage[currentIndex]);
    new (pointer) T(std::forward<Args>(args)...);
    return pointer;
  }

  /** @brief Destroy an object in the pool
   *
   * @param pointer Pointer to the object to be destroyed
   */
  void destroyObject(T *pointer)
  {
    if (pointer == nullptr)
    {
      return;
    }

    // verify pointer is in pool's memory range
    std::size_t indx = getIndex(pointer);
    if (indx >= N)
    {
      // invalid pointer
      return;
    }
    
    pointer->~T();

    // back to free list
    freeIndices[indx] = nextFree;
    nextFree = indx;
  }

private:
  // value indicating the end of the free list or invalid index
  static const std::size_t INVALID_INDEX = static_cast<std::size_t>(-1);
  // correct alignment for storage for type T
  using Storage = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
  // storage for N objects
  Storage storage[N];

  // free list array
  std::size_t freeIndices[N];

  // Index of the first free slot in freeIndices
  std::size_t nextFree;

  /**
   * @brief Get the index of the object in the pool
   *
   * @param pointer Pointer to the object
   * @return std::size_t Index of the object in the pool
   */
  std::size_t getIndex(T *pointer)
  {
    Storage *stPointer = reinterpret_cast<Storage *>(pointer);
    return stPointer - storage;
  }
};
