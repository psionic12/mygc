//
// Created by liu on 19-10-23.
//

#ifndef MYGC_GARBAGECOLLECTOR_H
#define MYGC_GARBAGECOLLECTOR_H

#define MYGC_HEAP_SIZE (1 << 22)


#include "../src/Heap.h"

#include <mutex>

namespace mygc {

class GarbageCollector {
 public:
  GarbageCollector() : mHeap(MYGC_HEAP_SIZE) {}
  void *New(size_t size);
 private:
  Heap mHeap;
  std::mutex mCollectMutex;
  void collect();
  void markFromRoots();
  void sweep();
};

} //namespace mygc
#endif //MYGC_GARBAGECOLLECTOR_H
