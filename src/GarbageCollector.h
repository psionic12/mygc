//
// Created by liu on 19-10-23.
//

#ifndef MYGC_GARBAGECOLLECTOR_H
#define MYGC_GARBAGECOLLECTOR_H

#include <mutex>
#include <set>
#include <condition_variable>

#include "YoungGeneration.h"
#include "TypeDescriptor.h"
#include "OldGeneration.h"
#include "YoungGenerationPool.h"

namespace mygc {
class GcReference;
class ObjectRecord;
class GarbageCollector {
 public:
  ObjectRecord *New(TypeDescriptor &descriptor);
  bool inHeap(void *ptr);
  void addRoots(GcReference *ptr);
  void removeRoots(GcReference *ptr);
  void attachThread(pthread_t thread);
  void detachThread(pthread_t thread);
  static GarbageCollector &getCollector();
  std::set<pthread_t> getAttachedThreads();
  void registerType(size_t id,
                    size_t typeSize,
                    std::pair<const size_t, const std::vector<size_t>> &&indices,
                    void (*destructor)(void *object) = nullptr);
  TypeDescriptor &getTypeById(size_t id);
 private:
  GarbageCollector();
  void collectStopped();
  void stopTheWorldLocked();
  void restartTheWorldLocked();
  std::mutex mGcMutex;
  std::set<GcReference *> mGcRoots;
  std::set<pthread_t> mAttachedThreads;
  std::map<size_t, TypeDescriptor> mTypeMap;
  OldGeneration mOldGeneration;
  YoungGenerationPool mYoungPool;
};

} //namespace mygc
#endif //MYGC_GARBAGECOLLECTOR_H
