//
// Created by liu on 19-10-25.
//

#ifndef MYGC_GC_PTR_H
#define MYGC_GC_PTR_H

#include <unordered_map>
#include <typeinfo>
#include <typeindex>

#include "GarbageCollector.h"
#include "CppDescriptor.h"

namespace mygc {

class PtrHeaders {
 public:
  bool check();
 private:
  uint8_t mUuid[16];
 protected:
  void *mPtr;
  PtrHeaders();
};

template<typename T, typename... _Args>
class gc_ptr : public PtrHeaders {
 private:
  static char sUuid[16];
  static GarbageCollector sGarbageCollector;
  static std::unordered_map<std::type_index, std::unique_ptr<IDescriptor>> sDescriptorMap;
 public:
  gc_ptr(_Args &&... args) {
    auto *ptr = sGarbageCollector.New(sizeof(T));
    ptr = new(ptr) T(std::forward<_Args>(args)...);
    try {
      sDescriptorMap.at(typeid(T)).get();
    } catch (const std::out_of_range &) {
      sDescriptorMap[typeid(T)] = std::make_unique<CppDescriptor<T>>(getGcPtrIndices((T *) ptr));
    }
    mPtr = ptr;
  }
  static std::vector<size_t> getGcPtrIndices(T *start) {
    std::vector<size_t> indices;
    auto size = sizeof(T);
    for (size_t i = 0; i < size;) {
      auto *header = reinterpret_cast<PtrHeaders *>((char *) start + i);
      if (header->check()) {
        indices.push_back(i);
        i += sizeof(PtrHeaders);
      } else {
        ++i;
      }
    }
    return indices;
  }
};
template<typename T, typename... Args>
GarbageCollector gc_ptr<T, Args...>::sGarbageCollector;

template<typename T, typename... Args>
std::unordered_map<std::type_index, std::unique_ptr<IDescriptor>> gc_ptr<T, Args...>::sDescriptorMap;

}//namespace mygc

#endif //MYGC_GC_PTR_H
