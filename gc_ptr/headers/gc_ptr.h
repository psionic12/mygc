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

#include "GarbageCollector.h"
#include "CppDescriptor.h"
#include "GcReference.h"

namespace mygc {

class CheckHeader {
 public:
  bool check();
 private:
  uint8_t mUuid[16];
 protected:
  CheckHeader();
};

class GcPtrHeader : public CheckHeader, public GcReference {
 public:
  GcPtrHeader(size_t size) : GcReference(size) {
    tThreadHandler; // used static thread_local member will not initiate(both on gcc and clang), so make it "used"
  }
 protected:
  static std::unordered_map<std::type_index, std::unique_ptr<IDescriptor>> sDescriptorMap;
  class ThreadHandler {
   public:
    ThreadHandler() {
      GarbageCollector::getCollector().attachThead(pthread_self());
      LOG(INFO) << "new thread " << pthread_self() << " created";
    }
    ~ThreadHandler() {
      GarbageCollector::getCollector().detachThead(pthread_self());
      LOG(INFO) << "thread " << pthread_self() << " destroyed";
    }
  };
  static thread_local ThreadHandler tThreadHandler;
};

template<typename T, typename... Args>
class gc_ptr : public GcPtrHeader {
 private:
  static char sUuid[16];
 public:
  gc_ptr(Args &&... args) : GcPtrHeader(sizeof(T)) {
    auto &collector = GarbageCollector::getCollector();
    auto ptr = getReference();
    ptr = new(ptr) T(std::forward<Args>(args)...);
    try {
      sDescriptorMap.at(typeid(T)).get();
    } catch (const std::out_of_range &) {
      sDescriptorMap[typeid(T)] = std::make_unique<CppDescriptor<T>>(getGcPtrIndices((T *) ptr));
    }
    if (!collector.inHeap(this)) {
      collector.addRoots(this);
    }
  }
  ~gc_ptr() {
    auto &collector = GarbageCollector::getCollector();
    if (!collector.inHeap(this)) {
      collector.removeRoots(this);
    }
  }
  static std::vector<size_t> getGcPtrIndices(T *start) {
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
};

}//namespace mygc

#endif //MYGC_GC_PTR_H
