//
// Created by liu on 19-10-23.
//

#ifndef MYGC_GARBAGECOLLECTOR_H
#define MYGC_GARBAGECOLLECTOR_H

#include <mutex>
#include <set>
#include <condition_variable>

#include "TypeDescriptor.h"
#include "OldGeneration.h"
#include "YoungGenerationPool.h"
#include "LargeObjects.h"
#include "GcReference.h"
#include "YoungGeneration.h"

namespace mygc {
class GarbageCollector {
 public:
  YoungRecord *New(ITypeDescriptor *descriptor);
  bool inHeap(void *ptr);
  void addRoots(GcReference *ptr);
  void removeRoots(GcReference *ptr);
  void attachThread(pthread_t thread);
  void detachThread(pthread_t thread);
  static GarbageCollector &getCollector();
  void registerType(size_t id,
                    size_t typeSize,
                    std::vector<size_t> &&indices,
                    void (*destructor)(void *object),
                    bool completed);
  void registerType(size_t id, size_t typeSize, size_t elementType, size_t counts);
  ITypeDescriptor *getTypeById(size_t id);
  bool willOom(size_t allocaSize);
  void updateTotalSize();
 private:
  GarbageCollector();
  void stopTheWorldLocked();
  void restartTheWorldLocked();
  void collectSTW();
  Record *collectRecordSTW(Record *root);
  void iterateChildren(mygc::SingleType *childType, Object *data);
  void iterateArray(mygc::ArrayType *arrayType, Object *data);
  std::mutex mGcMutex;
  std::set<GcReference *> mGcRoots;
  std::set<pthread_t> mAttachedThreads;
  std::map<size_t, std::unique_ptr<ITypeDescriptor>> mTypeMap;
  OldGeneration mOldGeneration;
  LargeObjects mLargeObjects;
  YoungGenerationPool mYoungPool;
  static thread_local std::unique_ptr<YoungGeneration> tYoung;
  YoungGeneration* getYoung();
  size_t mRemain;
 public:
  //only used for testing
  std::vector<size_t> getIndices(size_t typeId);
  std::set<pthread_t> getAttachedThreads();
  std::set<GcReference *> getRoots();
  void collect();
};

} //namespace mygc
#endif //MYGC_GARBAGECOLLECTOR_H
