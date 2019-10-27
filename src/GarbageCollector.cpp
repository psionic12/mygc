//
// Created by liu on 19-10-23.
//

#include "../includes/GarbageCollector.h"
void *mygc::GarbageCollector::New(size_t size) {
  auto *ref = mHeap.allocate(size);
  if (ref == nullptr) {
    collect();
    ref = mHeap.allocate(size);
    if (ref == nullptr) {
      throw std::runtime_error("out of memory");
    }
  }
  return ref;
}
void mygc::GarbageCollector::collect() {
  std::lock_guard<std::mutex> guard(mCollectMutex);
}
