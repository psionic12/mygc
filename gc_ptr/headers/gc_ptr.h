//
// Created by liu on 19-10-25.
//

#ifndef MYGC_GC_PTR_H
#define MYGC_GC_PTR_H

#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <memory>
#include <glog/logging.h>

#include "GcReference.h"

namespace mygc {
template<typename T, typename... Args>
class gc_ptr {
 public:
  gc_ptr(Args &&... args) {
    auto ptr = new() T(std::forward<Args>(args)...);
  }
};

template<typename T, typename... Args, size_t SIZE>
class gc_ptr<T[SIZE], Args...> {
 public:
  gc_ptr(Args &&... args) {
    auto ptr = new() T[SIZE]{std::forward<Args>(args)...};
  }
};

}//namespace mygc

#endif //MYGC_GC_PTR_H
