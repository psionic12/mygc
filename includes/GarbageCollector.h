//
// Created by liu on 19-10-23.
//

#ifndef MYGC_GARBAGECOLLECTOR_H
#define MYGC_GARBAGECOLLECTOR_H

#include <mutex>
#include <set>
#include <condition_variable>

#include "../src/Heap.h"

namespace mygc {

class GarbageCollector {
 public:
  void *New(size_t size);
  bool inHeap(void *ptr);
  void addRoots(void *ptr);
  void removeRoots(void *ptr);
  void attachThead(pthread_t thread);
  void detachThead(pthread_t thread);
  static GarbageCollector &getCollector();
  std::set<pthread_t> getAttachedThreads();
 private:
  GarbageCollector();
  void collectLocked();
  void stopTheWorldLocked();
  void restartTheWorldLocked();
  Heap mHeap;
  std::mutex mGcMutex;
  std::set<void *> mGcRoots;
  static std::set<pthread_t> sAttachedThreads;
};

} //namespace mygc
#endif //MYGC_GARBAGECOLLECTOR_H
