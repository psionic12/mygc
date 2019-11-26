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

mygc::GarbageCollector::GarbageCollector() : mYoungGeneration(mOldGeneration, mGcRoots) {
  stop_the_world_init();
  // initial glog
//  google::InitGoogleLogging(nullptr);
  FLAGS_logtostderr = true;
}

mygc::ObjectRecord *mygc::GarbageCollector::New(TypeDescriptor &descriptor) {
  std::lock_guard<std::mutex> guard(mGcMutex);
  auto *ptr = mYoungGeneration.allocateLocked(descriptor);
  if (!ptr) {
    stop_the_world(mAttachedThreads);
    collectStopped();
    mOldGeneration.onCollectionFinished();
    mYoungGeneration.onCollectionFinished();
    restart_the_world();
    ptr = mYoungGeneration.allocateLocked(descriptor);
    if (!ptr) {
      throw std::runtime_error("mygc is out of memory");
    }
  }
  return ptr;
}
void mygc::GarbageCollector::collectStopped() {
  mYoungGeneration.collectStopped();
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
