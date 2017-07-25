// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// Simple hash function used for internal data structures

#ifndef STORAGE_LEVELDB_UTIL_HASH_H_
#define STORAGE_LEVELDB_UTIL_HASH_H_

#include <stddef.h>

#if defined(HAVE_STDINT_H) && HAVE_STDINT_H == 1
#  include <stdint.h>
#elif defined(HAVE_INTTYPES_H) && HAVE_INTTYPES_H == 1
#  include <inttypes.h>
#endif

namespace leveldb {

extern uint32_t Hash(const void * data, size_t n, uint32_t seed);

}

#endif  // STORAGE_LEVELDB_UTIL_HASH_H_
