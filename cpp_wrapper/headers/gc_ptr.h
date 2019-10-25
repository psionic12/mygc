//
// Created by liu on 19-10-25.
//

#ifndef MYGC_GC_PTR_H
#define MYGC_GC_PTR_H

#include <map>

#include "GarbageCollector.h"
#include "CppDescriptor.h"

namespace mygc {

template<typename T, typename... _Args>
class gc_ptr {
 private:
  static GarbageCollector sGarbageCollector;
  static std::map<std::type_info, std::unique_ptr<IDescriptor>> sDescriptorMap;
 public:
  // do not make this static, we need this field exists on every object and be the first member
  GarbageCollector *mGarbageCollector = &sGarbageCollector;
 public:
  gc_ptr(_Args &&... args) {
    auto *ptr = sGarbageCollector.New(sizeof(T));
    ptr = new(ptr) T(std::forward<_Args>(args)...);
    IDescriptor *descriptor = nullptr;
//    try {
//      descriptor = &sDescriptorMap.at(typeid(T));
//    } catch (const std::out_of_range &) {
//
//      sDescriptorMap.insert(typeid(T),
//                            std::make_unique<CppDescriptor<T>>());
//    }
  }
  static std::vector<size_t> getGcPtrIndices(T *start) {
    std::vector<size_t> indices;
    for (size_t i = 0; i < sizeof(T); i++) {
      void *p = *reinterpret_cast<void **>(start + i);
      void *q = &sGarbageCollector;
      if (p == &sGarbageCollector) {
        indices.push_back(i);
      }
    }
    return indices;
  }
};
template<typename T, typename... Args>
GarbageCollector gc_ptr<T, Args...>::sGarbageCollector;

template<typename T, typename... Args>
std::map<std::type_info, std::unique_ptr<IDescriptor>> gc_ptr<T, Args...>::sDescriptorMap;

}//namespace mygc

#endif //MYGC_GC_PTR_H
