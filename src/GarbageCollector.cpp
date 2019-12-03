//
// Created by liu on 19-10-23.
//

#include <signal.h>
#include <glog/logging.h>
#include "GarbageCollector.h"
#include "stop_the_world.h"

#ifndef MYGC_STOP_SIGNAL
#define MYGC_STOP_SIGNAL SIGRTMIN + ('m' + 'y' + 'g' + 'c') % (SIGRTMAX - SIGRTMIN)
#endif

thread_local std::unique_ptr<mygc::YoungGeneration>
    mygc::GarbageCollector::tYoungGeneration(mygc::GarbageCollector::getCollector().mYoungPool.getCleanGeneration());
mygc::GarbageCollector::GarbageCollector() {
  stop_the_world_init();
  // initial glog
//  google::InitGoogleLogging(nullptr);
  FLAGS_logtostderr = true;
}

mygc::YoungRecord *mygc::GarbageCollector::New(TypeDescriptor &descriptor) {
  std::lock_guard<std::mutex> guard(mGcMutex);
  auto *ptr = tYoungGeneration->allocate(descriptor);
  if (!ptr) {
    stopTheWorldLocked();
    collectSTW();
    restartTheWorldLocked();
    ptr = tYoungGeneration->allocate(descriptor);
    if (!ptr) {
      throw std::runtime_error("mygc is out of memory");
    }
  }
  return ptr;
}
void mygc::GarbageCollector::addRoots(GcReference *ptr) {
  std::lock_guard<std::mutex> guard(mGcMutex);
  mGcRoots.insert(ptr);
}
void mygc::GarbageCollector::removeRoots(GcReference *ptr) {
  std::lock_guard<std::mutex> guard(mGcMutex);
  mGcRoots.erase(ptr);
}
void mygc::GarbageCollector::attachThread(pthread_t thread) {
  std::lock_guard<std::mutex> guard(mGcMutex);
  mAttachedThreads.emplace(thread);
}
void mygc::GarbageCollector::detachThread(pthread_t thread) {
  std::lock_guard<std::mutex> guard(mGcMutex);
  mAttachedThreads.erase(thread);
}
mygc::GarbageCollector &mygc::GarbageCollector::getCollector() {
  static GarbageCollector garbageCollector;
  return garbageCollector;
}
void mygc::GarbageCollector::stopTheWorldLocked() {
  stop_the_world(mAttachedThreads);
}
void mygc::GarbageCollector::restartTheWorldLocked() {
  restart_the_world();
}
std::set<pthread_t> mygc::GarbageCollector::getAttachedThreads() {
  std::lock_guard<std::mutex> guard(mGcMutex);
  return mAttachedThreads;
}
void mygc::GarbageCollector::registerType(size_t id,
                                          size_t typeSize,
                                          std::pair<const size_t, const std::vector<size_t>> &&indices,
                                          void (*destructor)(void *)) {
  std::lock_guard<std::mutex> guard(mGcMutex);
  try {
    mTypeMap.at(id);
  } catch (const std::out_of_range &) {
    mTypeMap.insert({id, {typeSize, std::move(indices), destructor}});
  }
}
mygc::TypeDescriptor &mygc::GarbageCollector::getTypeById(size_t id) {
  return mTypeMap.at(id);
}

mygc::Record *mygc::GarbageCollector::collectRecordSTW(Record *root) {
  Object *data = nullptr;
  Record *handledRecord = nullptr;
  switch (root->location) {
    case Location::kYoungGeneration: {
      auto *young = (YoungRecord *) root;
      OldRecord *old;
      if (!young->copied) {
        old = mOldGeneration.copyFromYoungSTW(young);
        young->copied = true;
        if (young->descriptor->nonTrivial()) {
          young->generation->getFinalizerList().remove(young);
        }
        young->forwardAddress = old;
      } else {
        old = young->forwardAddress;
      }
      data = old->data;
      handledRecord = old;
      break;
    }
    case Location::kOldGeneration: {
      auto *old = (OldRecord *) root;
      mOldGeneration.mark(old);
      break;
    }
    case Location::kLargeObjects: {
      auto *large = (LargeRecord *) root;
      mLargeObjects.mark(large);
      break;
    }
  }
  // handle children
  const auto &pair = handledRecord->descriptor->getIndices();
  size_t group = pair.first; // big than 1 if it is an array
  const std::vector<size_t> &indices = pair.second;
  //OPT: most objects are not arrays
  for (int i = 0; i < group; i++) {
    auto *arrayData = data + i;
    for (auto index : indices) {
      auto *ref = (GcReference *) (arrayData + index);
      auto *childHandledRecord = collectRecordSTW(ref->getRecord());
      ref->update(childHandledRecord);
    }
  }
  return handledRecord;
}
void mygc::GarbageCollector::collectSTW() {
  LOG(INFO) << "start collecting" << std::endl;
  for (auto *ref : mGcRoots) {
    auto *record = ref->getRecord();
    if (record) {
      auto *ptr = collectRecordSTW(record);
      ref->update(ptr);
    }
  }
  mOldGeneration.onScanEnd();
  mLargeObjects.onScanEnd();
  mYoungPool.putDirtyGeneration(std::move(tYoungGeneration));
  tYoungGeneration = mYoungPool.getCleanGeneration();
  LOG(INFO) << "collecting finished" << std::endl;
}
bool mygc::GarbageCollector::inHeap(void *ptr) {
  return tYoungGeneration->inHeapLocked(ptr);
}
