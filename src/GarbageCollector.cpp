//
// Created by liu on 19-10-23.
//

#include <signal.h>
#include <glog/logging.h>
#include "GarbageCollector.h"
#include "stop_the_world.h"
#include "Tester.h"

#ifndef MYGC_STOP_SIGNAL
#define MYGC_STOP_SIGNAL SIGRTMIN + ('m' + 'y' + 'g' + 'c') % (SIGRTMAX - SIGRTMIN)
#endif

mygc::GarbageCollector::GarbageCollector() {
  stop_the_world_init();
  // initial glog
//  google::InitGoogleLogging(nullptr);
  FLAGS_logtostderr = true;

  LOG(INFO) << "mygc: " << std::endl;
  LOG(INFO) << "stop signal: " << MYGC_STOP_SIGNAL << std::endl;
  LOG(INFO) << "young generation heap size: " << YoungGeneration::defaultSize() << std::endl;
}

mygc::YoungRecord *mygc::GarbageCollector::New(ITypeDescriptor *descriptor) {
  auto *ptr = mYoungGenerations.getMine()->allocate(descriptor);
  if (!ptr) {
    {
      std::lock_guard<std::mutex> guard(mGcMutex);
      // try again in case a gc has finished
      ptr = mYoungGenerations.getMine()->allocate(descriptor);
      if (!ptr) {
        stopTheWorldLocked();
        collectSTW();
        restartTheWorldLocked();
      }
    }
    ptr = mYoungGenerations.getMine()->allocate(descriptor);
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
                                          std::vector<size_t> &&indices,
                                          void (*destructor)(void *),
                                          bool completed) {
  std::lock_guard<std::mutex> guard(mGcMutex);
  try {
    SingleType *descriptor = (SingleType *) mTypeMap.at(id).get();
    descriptor->update(typeSize, std::move(indices), destructor, completed);
  } catch (const std::out_of_range &) {
    mTypeMap.insert({id, std::make_unique<SingleType>(typeSize, std::move(indices), destructor, completed)});
  }
}
void mygc::GarbageCollector::registerType(size_t id, size_t typeSize, size_t elementType, size_t counts) {
  try {
    ArrayType *descriptor = (ArrayType *) mTypeMap.at(id).get();
    descriptor->update(typeSize, getTypeById(elementType), counts);
  } catch (const std::out_of_range &) {
    mTypeMap.insert({id, std::make_unique<ArrayType>(typeSize, getTypeById(elementType), counts)});
  }
}
mygc::ITypeDescriptor *mygc::GarbageCollector::getTypeById(size_t id) {
  return mTypeMap.at(id).get();
}
mygc::Record *mygc::GarbageCollector::collectRecordSTW(Record *root) {
  Object *data = nullptr;
  Record *handledRecord = nullptr;
  switch (root->location) {
    case Location::kYoungGeneration: {
      auto *young = (YoungRecord *) root;
      OldRecord *old;
      if (!young->copied) {
//        DLOG(INFO) << "copy alive objects " << ((Tester *) (young->data))->mId << std::endl;
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
      mOldGeneration.scan(old);
      break;
    }
    case Location::kLargeObjects: {
      auto *large = (LargeRecord *) root;
      mLargeObjects.mark(large);
      break;
    }
  }

  auto *descriptor = handledRecord->descriptor;
  if (auto *singleType = dynamic_cast<SingleType *>(descriptor)) {
    iterateChildren(singleType, data);
  } else if (auto *arrayType = dynamic_cast<ArrayType *>(descriptor)) {
    iterateArray(arrayType, data);
  } else {
    throw std::runtime_error("unknown type");
  }

  return handledRecord;
}
void mygc::GarbageCollector::iterateChildren(mygc::SingleType *childType, Object *data) {
  const auto &indices = childType->getIndices();
  for (auto index : indices) {
    auto *ref = (GcReference *) (data + index);
    auto *childHandledRecord = collectRecordSTW(ref->getRecord());
    ref->update(childHandledRecord);
  }
}
void mygc::GarbageCollector::iterateArray(mygc::ArrayType *arrayType, mygc::Object *data) {
  auto counts = arrayType->getCounts();
  for (int i = 0; i < counts; i++) {
    Object *childData = data + i * arrayType->typeSize();
    auto *elementType = arrayType->getElementType();
    if (auto *singleType = dynamic_cast<SingleType *>(elementType)) {
      iterateChildren(singleType, childData);
    } else if (auto *childArrayType = dynamic_cast<ArrayType *>(elementType)) {
      iterateArray(childArrayType, childData);
    } else {
      throw std::runtime_error("unknown type");
    }
  }
}
void mygc::GarbageCollector::collectSTW() {
//  LOG(INFO) << "start collecting" << std::endl;
  for (auto *ref : mGcRoots) {
    auto *record = ref->getRecord();
    if (record) {
      auto *ptr = collectRecordSTW(record);
      ref->update(ptr);
    }
  }
  mOldGeneration.onScanEnd();
  mLargeObjects.onScanEnd();
  mYoungGenerations.onScanEnd();
//  LOG(INFO) << "collecting finished" << std::endl;
}
bool mygc::GarbageCollector::inHeap(void *ptr) {
  return mYoungGenerations.getMine()->inHeapLocked(ptr);
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

extern "C" void __cxa_pure_virtual() {
  DLOG(INFO) << "__cxa_pure_virtual" << std::endl;
}
