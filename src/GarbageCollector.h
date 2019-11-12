//
// Created by liu on 19-10-23.
//

#ifndef MYGC_GARBAGECOLLECTOR_H
#define MYGC_GARBAGECOLLECTOR_H

#include <mutex>
#include <set>
#include <condition_variable>

#include "Heap.h"
#include "YoungGeneration.h"

namespace mygc {
class GcReference;
class ObjectRecord;
class GarbageCollector {
 public:
  ObjectRecord * New(size_t size);
  bool inHeap(void *ptr);
  void addRoots(GcReference *ptr);
  void removeRoots(GcReference *ptr);
  void attachThread(pthread_t thread);
  void detachThread(pthread_t thread);
  static GarbageCollector &getCollector();
  std::set<pthread_t> getAttachedThreads();
 private:
  GarbageCollector();
  void collectLocked();
  void stopTheWorldLocked();
  void restartTheWorldLocked();
  std::mutex mGcMutex;
  std::set<GcReference *> mGcRoots;
  static std::set<pthread_t> sAttachedThreads;
};

} //namespace mygc
#endif //MYGC_GARBAGECOLLECTOR_H
