/**
 * @file   tiledb.h
 *
 * @author Ravi Gaddipati
 *
 * @section LICENSE
 *
 * The MIT License
 *
 * @copyright Copyright (c) 2017 TileDB, Inc.
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
 * This file declares the C++ API for TileDB.
 */

#ifndef TILEDB_GENOMICS_ATTRIBUTE_H
#define TILEDB_GENOMICS_ATTRIBUTE_H

#include "tdbpp_object.h"
#include "tdbpp_context.h"
#include "tdbpp_type.h"
#include "tiledb.h"

#include <functional>
#include <memory>

namespace tdb {

  class Attribute {
  public:
    Attribute(Context &ctx) : _ctx(ctx), _deleter(ctx) {}
    Attribute(Context &ctx, tiledb_attribute_t **attr) : Attribute(ctx) {
      load(attr);
    }
    Attribute(Context &ctx, std::string name, tiledb_datatype_t type) : Attribute(ctx) {
      create(name, type);
    }
    Attribute(const Attribute &attr) = default;
    Attribute(Attribute &&o) = default;
    Attribute &operator=(const Attribute&) = default;
    Attribute &operator=(Attribute&& o) = default;

    void load(tiledb_attribute_t **attr) {
      if (attr && *attr) {
        _init(*attr);
        *attr = nullptr;
      }
    }

    template<typename DataT>
    Attribute &create(const std::string &name) {
      _create(name, DataT::tiledb_datatype);
      return *this;
    }

    Attribute &create(const std::string &name, tiledb_datatype_t type) {
      _create(name, type);
      return *this;
    }


    std::string name() const;

    tiledb_datatype_t type() const;

    unsigned num() const;

    Attribute &set_num(unsigned num);

    Compressor compressor() const;

    Attribute &set_compressor(Compressor c);

    std::shared_ptr<tiledb_attribute_t> ptr() const {
      return _attr;
    }

  protected:
    struct _Deleter {
      _Deleter(Context& ctx) : _ctx(ctx) {}
      _Deleter(const _Deleter&) = default;
      void operator()(tiledb_attribute_t *p);
    private:
      std::reference_wrapper<Context> _ctx;
    };
    void _init(tiledb_attribute_t *attr);
    void _create(const std::string &name, tiledb_datatype_t type);

    std::reference_wrapper<Context> _ctx;
    _Deleter _deleter;
    std::shared_ptr<tiledb_attribute_t> _attr;
  };

}

std::ostream &operator<<(std::ostream &os, const tdb::Attribute &a);
tdb::Attribute &operator<<(tdb::Attribute &attr, const tdb::Compressor &c);

#endif //TILEDB_GENOMICS_ATTRIBUTE_H
