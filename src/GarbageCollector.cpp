//
// Created by liu on 19-10-23.
//

#include <signal.h>
#include "GarbageCollector.h"
#include "stop_the_world.h"
#include "Tools.h"

#ifndef MYGC_STOP_SIGNAL
#define MYGC_STOP_SIGNAL (SIGRTMIN + ('m' + 'y' + 'g' + 'c') % (SIGRTMAX - SIGRTMIN))
#endif

#ifndef MYGC_TOTAL_SIZE
#define MYGC_TOTAL_SIZE (2 << 29)
#endif

thread_local std::unique_ptr<mygc::YoungGeneration> mygc::GarbageCollector::tYoung;
mygc::GarbageCollector::GarbageCollector() : mRemain(MYGC_TOTAL_SIZE) {
  stop_the_world_init();
  // initial glog
//  google::InitGoogleLogging(nullptr);
//  FLAGS_logtostderr = true;
//  google::InstallFailureSignalHandler();

//  LOG(INFO) << "mygc: " << std::endl;
//  LOG(INFO) << "stop signal: " << MYGC_STOP_SIGNAL << std::endl;
//  LOG(INFO) << "young generation heap size: " << YoungGeneration::defaultSize() << std::endl;
}

mygc::Record *mygc::GarbageCollector::New(ITypeDescriptor *descriptor, size_t counts) {
  size_t size = sizeof(YoungRecord) + counts * descriptor->typeSize();
  if (size < OldGeneration::getMaxBlockSize()) {
    auto *ptr = getYoung()->allocate(descriptor, counts);
    if (!ptr) {
      {
        std::lock_guard<std::mutex> guard(mGcMutex);
        // try again in case a gc has finished
        ptr = getYoung()->allocate(descriptor, counts);
        if (!ptr) {
          stopTheWorldLocked();
          collectSTW();
          restartTheWorldLocked();
        }
      }
      ptr = getYoung()->allocate(descriptor, counts);
      if (!ptr) {
        throw std::runtime_error("mygc is out of memory");
      }
    }
    return ptr;
  } else {
    std::lock_guard<std::mutex> guard(mGcMutex);
    return mLargeObjects.allocate(descriptor, counts);
  }
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
                                          std::vector<size_t> &&indices,
                                          void (*destructor)(void *),
                                          bool completed) {
  std::lock_guard<std::mutex> guard(mGcMutex);
  try {
    auto *descriptor = (SingleType *) mTypeMap.at(id).get();
    descriptor->update(std::move(indices), completed);
  } catch (const std::out_of_range &) {
    mTypeMap.insert({id, std::make_unique<SingleType>(typeSize, std::move(indices), destructor, completed)});
  }
}
mygc::ITypeDescriptor *mygc::GarbageCollector::getTypeById(size_t id) {
  return mTypeMap.at(id).get();
}
mygc::Record *mygc::GarbageCollector::collectRecordSTW(Record *root) {
  if (!root) return nullptr;
  Object *data = nullptr;
  Record *handledRecord = nullptr;
  switch (root->location) {
    case Location::kYoungGeneration: {
      auto *young = (YoungRecord *) root;
      OldRecord *old;
      if (!young->copied) {
        young->copied = true;
        if (!inHeap(young)) {
          //we do not move object in other young generations, but we should iterate the child.
          handledRecord = young;
          break;
        } else {
          old = mOldGeneration.copyFromYoungSTW(young);
          if (young->descriptor->nonTrivial()) {
            young->generation->getFinalizerList().remove(young);
          }
          young->forwardAddress = old;
          data = old->data;
          handledRecord = old;
          break;
        }
      } else {
        return young->forwardAddress;
      }
    }
    case Location::kOldGeneration: {
      auto *old = (OldRecord *) root;
      if (mOldGeneration.mark(old)) {
        mOldGeneration.pickNonTrivial(old);
        handledRecord = old;
        break;
      } else {
        return old;
      }
    }
    case Location::kLargeObjects: {
      auto *large = (LargeRecord *) root;
      mLargeObjects.mark(large);
      handledRecord = large;
      data = large->data;
      break;
    }
  }

  auto *singleType = dynamic_cast<SingleType *>(handledRecord->descriptor);
  for (int i = 0; i < handledRecord->counts; i++) {
    iterateChildren(singleType, data + (i * singleType->typeSize()));
  }
  return handledRecord;
}
void mygc::GarbageCollector::iterateChildren(mygc::SingleType *childType, Object *data) {
  if (!data) return;
  const auto &indices = childType->getIndices();
  for (auto index : indices) {
    auto *ref = (GcReference *) (data + index);
    auto *childHandledRecord = collectRecordSTW(ref->getRecord());
    ref->update(childHandledRecord);
  }
}
void mygc::GarbageCollector::collectSTW() {
//  GCLOG("start collecting");
  for (auto *ref : mGcRoots) {
    auto *record = ref->getRecord();
    if (record) {
      auto *ptr = collectRecordSTW(record);
      ref->update(ptr);
    }
  }
  mOldGeneration.onScanEnd();
  mLargeObjects.onScanEnd();
  if (tYoung) {
    if (tYoung->getFinalizerList().getHead()) {
      mYoungPool.putDirtyGeneration(std::move(tYoung));
      tYoung = mYoungPool.getCleanGeneration();
    } else {
      tYoung->reset();
    }
  }

//  GCLOG("collecting finished");
}
bool mygc::GarbageCollector::inHeap(void *ptr) {
  return getYoung()->inHeapLocked(ptr);
}
std::vector<size_t> mygc::GarbageCollector::getIndices(size_t typeId) {
  std::lock_guard<std::mutex> guard(mGcMutex);
  auto *descriptor = mTypeMap.at(typeId).get();
  if (auto *singleType = dynamic_cast<SingleType *>(descriptor)) {
    return singleType->getIndices();
  } else {
    throw std::runtime_error("get indices for an array type");
  }
}
std::set<mygc::GcReference *> mygc::GarbageCollector::getRoots() {
  std::lock_guard<std::mutex> guard(mGcMutex);
  return mGcRoots;
}
mygc::YoungGeneration *mygc::GarbageCollector::getYoung() {
  if (!tYoung) {
    tYoung = std::make_unique<YoungGeneration>();
  }
  return tYoung.get();
}
bool mygc::GarbageCollector::willOom(size_t allocaSize) {
  return mRemain < allocaSize;
}
void mygc::GarbageCollector::updateTotalSizeSTW() {
  mRemain = MYGC_TOTAL_SIZE - mOldGeneration.getAllocatedSize();
}
void mygc::GarbageCollector::collect() {
  std::lock_guard<std::mutex> guard(mGcMutex);
  stopTheWorldLocked();
  collectSTW();
  restartTheWorldLocked();
}
bool mygc::GarbageCollector::isCompletedDescriptor(size_t typeId) {
  std::lock_guard<std::mutex> guard(mGcMutex);
  return getTypeById(typeId)->isCompleted();
}
