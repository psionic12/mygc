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

#include "../../src/GarbageCollector.h"
#include "CppDescriptor.h"
#include "GcReference.h"

namespace mygc {

class CheckHeader {
 public:
  bool check();
 private:
  uint8_t mUuid[16];
  static char sUuid[16];
 protected:
  CheckHeader();
};

class GcPtrHeader : public CheckHeader, public GcReference {
 public:
  GcPtrHeader(size_t size) : GcReference(size) {
    tThreadHandler; // used static thread_local member will not initiate(both on gcc and clang), so make it "used"
    if (!GarbageCollector::getCollector().inHeap(this)) {
      GarbageCollector::getCollector().addRoots(this);
    }
  }
  virtual ~GcPtrHeader() {
    auto &collector = GarbageCollector::getCollector();
    if (!collector.inHeap(this)) {
      collector.removeRoots(this);
    }
  }
 protected:
  static std::unordered_map<std::type_index, std::unique_ptr<IDescriptor>> sDescriptorMap;
  class ThreadHandler {
   public:
    ThreadHandler() {
      GarbageCollector::getCollector().attachThread(pthread_self());
      LOG(INFO) << "new thread " << pthread_self() << " created";
    }
    ~ThreadHandler() {
      GarbageCollector::getCollector().detachThread(pthread_self());
      LOG(INFO) << "thread " << pthread_self() << " destroyed";
    }
  };
  static thread_local ThreadHandler tThreadHandler;
};

template<typename T>
std::vector<size_t> getGcPtrIndices(T *start) {
  std::vector<size_t> indices;
  auto size = sizeof(T);
  for (size_t i = 0; i < size;) {
    auto *header = reinterpret_cast<GcPtrHeader *>((char *) start + i);
    if (header->check()) {
      indices.push_back(i);
      i += sizeof(GcPtrHeader);
    } else {
      ++i;
    }
  }
  return indices;
}

template<typename T, typename... Args>
class gc_ptr : public GcPtrHeader {
 private:
  static char sUuid[16];
 public:
  gc_ptr(Args &&... args) : GcPtrHeader(sizeof(T)) {
    auto ptr = new(getReference()) T(std::forward<Args>(args)...);
    try {
      sDescriptorMap.at(typeid(T)).get();
    } catch (const std::out_of_range &) {
      sDescriptorMap[typeid(T)] = std::make_unique<CppDescriptor<T>>(getGcPtrIndices(ptr));
    }
  }
};

template<typename T, typename... Args, size_t SIZE>
class gc_ptr<T[SIZE], Args...> : public GcPtrHeader {
 public:
  gc_ptr(Args &&... args) : GcPtrHeader(sizeof(T) * SIZE) {
    auto ptr = new(getReference()) T[SIZE]{std::forward<Args>(args)...};
    try {
      sDescriptorMap.at(typeid(T[SIZE])).get();
    } catch (const std::out_of_range &) {
      try {
        sDescriptorMap.at(typeid(T)).get();
      } catch (const std::out_of_range &) {
        sDescriptorMap[typeid(T)] = std::make_unique<CppDescriptor<T>>(getGcPtrIndices(ptr));
      }
      sDescriptorMap[typeid(T[SIZE])] = std::make_unique<CppDescriptor<T[SIZE]>>(sDescriptorMap.at(typeid(T)).get());
    }
  }
};

}//namespace mygc

#endif //MYGC_GC_PTR_H
