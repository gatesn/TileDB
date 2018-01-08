/**
 * @file   tdbpp_urils.h
 *
 * @section LICENSE
 *
 * The MIT License
 *
 * @copyright Copyright (c) 2017 TileDB, Inc.
 * @copyright Copyright (c) 2016 MIT and Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 * Utils for C++ API.
 */

#ifndef TILEDB_TDBPP_UTILS_H
#define TILEDB_TDBPP_UTILS_H

#include "tiledb.h"
#include <iostream>
#include <functional>
#include <array>

namespace tdb {

  struct Version {
    int major, minor, rev;
  };

  /**
   * @return TileDB library version
   */
  inline const Version version() {
    Version ret;
    tiledb_version(&ret.major, &ret.minor, &ret.rev);
    return ret;
  }

  /**
 * Covert an offset, data vector pair into a single vector of vectors.
   *
 * @tparam T underlying datatype
 * @tparam E element type. usually std::vector<T> or std::string. Must be constructable by a buff::iterator pair
 * @param offsets Offsets vector
 * @param buff data vector
 * @param num_offset num offset elements populated by query
 * @param num_buff num data elements populated by query.
 * @return std::vector<E>
 */
  template <typename T, typename E=typename std::vector<T>>
  std::vector<E> group_by_cell(const std::vector<uint64_t> &offsets, const std::vector<T> &buff,
                               uint64_t num_offset, uint64_t num_buff) {
    std::vector<E> ret;
    ret.reserve(num_offset);
    for (unsigned i = 0; i < num_offset; ++i) {
      ret.emplace_back(buff.begin() + offsets[i], (i == num_offset - 1) ? buff.begin()+num_buff : buff.begin()+offsets[i+1]);
    }
    return ret;
  }

  /**
   * Covert an offset, data vector pair into a single vector of vectors.
   *
   * @tparam T underlying datatype
   * @tparam E element type. usually std::vector<T> or std::string. Must be constructable by a buff::iterator pair
   * @param buff pair<offset_vec, data_vec>
   * @param num_offset num offset elements populated by query
   * @param num_buff num data elements populated by query.
   * @return std::vector<E>
   */
  template <typename T, typename E=typename std::vector<T>>
  std::vector<E> group_by_cell(const std::pair<std::vector<uint64_t>, std::vector<T>> &buff,
                               uint64_t num_offset, uint64_t num_buff) {
    return group_by_cell<T,E>(buff.first, buff.second, num_offset, num_buff);
  }

  /**
   * Group by cell at runtime.
   *
   * @tparam T Element type
   * @tparam E element type. usually std::vector<T> or std::string. Must be constructable by a buff::iterator pair
   * @param buff data buffer
   * @param el_per_cell Number of elements per cell to group together
   * @param num_buff Number of elements populated by query. To group whole buffer, use buff.size()
   */
  template<typename T, typename E=typename std::vector<T>>
  std::vector<E> group_by_cell(const std::vector<T> &buff, uint64_t el_per_cell, uint64_t num_buff) {
    std::vector<E> ret;
    if (buff.size() % el_per_cell != 0) {
      throw std::invalid_argument("Buffer is not a multiple of elements per cell.");
    }
    ret.reserve(buff.size()/el_per_cell);
    for (uint64_t i = 0; i < num_buff; i+= el_per_cell) {
      ret.emplace_back(buff.begin() + i, buff.begin() + i + el_per_cell);
    }
    return ret;
  }

  /**
   * Group a data vector into a a vector of arrays
   *
   * @tparam N Elements per cell
   * @tparam T Array element type
   * @param buff data buff to group
   * @param num_buff Number of elements in buff that were populated by the query.
   * @return std::vector<std::array<T,N>>
   */
  template<uint64_t N, typename T>
  std::vector<std::array<T,N>> group_by_cell(const std::vector<T> &buff, uint64_t num_buff) {
    std::vector<std::array<T,N>> ret;
    if (buff.size() % N != 0) {
      throw std::invalid_argument("Buffer is not a multiple of elements per cell.");
    }
    ret.reserve(buff.size()/N);
    for (unsigned i = 0; i < num_buff; i+= N) {
      std::array<T,N> a;
      for (unsigned j = 0; j < N; ++j) {
        a[j] = buff[i+j];
      }
      ret.insert(ret.end(), std::move(a));
    }
    return ret;
  }


  /**
   * Unpack a vector of variable sized attributes into a data and offset buffer.
   *
   * @tparam T Vector type. T::value_type is considered the underlying data element type. Should be vector or string.
   * @tparam R T::value_type, deduced
   * @param data data to unpack
   * @return pair where .first is the offset buffer, and .second is data buffer
   */
  template<typename T, typename R=typename T::value_type>
  std::pair<std::vector<uint64_t>, std::vector<R>> make_var_buffers(const std::vector<T> &data) {
    std::pair<std::vector<uint64_t>, std::vector<R>> ret;
    ret.first.push_back(0);
    for (const auto &v : data) {
      ret.second.insert(std::end(ret.second), std::begin(v), std::end(v));
      ret.first.push_back(ret.first.back() + v.size());
    }
    ret.first.pop_back();
    return ret;
  };

}

inline std::ostream &operator<<(std::ostream &os, const tdb::Version &v) {
  os << "TileDB v" << v.major << '.' << v.minor << '.' << v.rev;
  return os;
}

#endif //TILEDB_TDBPP_UTILS_H
