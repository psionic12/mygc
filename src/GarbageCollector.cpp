//
// Created by liu on 19-10-23.
//

#include <signal.h>
#include <glog/logging.h>
#include <GarbageCollector.h>

#include "GarbageCollector.h"
#include "stop_the_world.h"

#define MYGC_HEAP_SIZE (1 << 22)

#ifndef MYGC_STOP_SIGNAL
#define MYGC_STOP_SIGNAL SIGRTMIN + ('m' + 'y' + 'g' + 'c') % (SIGRTMAX - SIGRTMIN)
#endif

mygc::GarbageCollector mygc::GarbageCollector::sGarbageCollector;
std::set<pthread_t> mygc::GarbageCollector::sAttachedThreads;

mygc::GarbageCollector::GarbageCollector()
    : mHeap(MYGC_HEAP_SIZE) {
  stop_the_world_init();
  // initial glog
  google::InitGoogleLogging(nullptr);
  FLAGS_logtostderr = 1;
}

void *mygc::GarbageCollector::New(size_t size) {
  std::lock_guard<std::mutex> guard(mGcMutex);
  auto *ref = mHeap.allocateLocked(size);
  if (ref == nullptr) {
    collectLocked();
    ref = mHeap.allocateLocked(size);
    if (ref == nullptr) {
      throw std::runtime_error("out of memory");
    }
  }
  return ref;
}
void mygc::GarbageCollector::collectLocked() {
}
bool mygc::GarbageCollector::inHeap(void *ptr) {
  std::lock_guard<std::mutex> guard(mGcMutex);
  return mHeap.inHeapLocked(ptr);
}
void mygc::GarbageCollector::addRoots(void *ptr) {
  std::lock_guard<std::mutex> guard(mGcMutex);
  mGcRoots.insert(ptr);
}
void mygc::GarbageCollector::removeRoots(void *ptr) {
  std::lock_guard<std::mutex> guard(mGcMutex);
  mGcRoots.erase(ptr);
}
void mygc::GarbageCollector::attachThead(pthread_t thread) {
  sAttachedThreads.emplace(thread);
}
void mygc::GarbageCollector::detachThead(pthread_t thread) {
  sAttachedThreads.erase(thread);
}
mygc::GarbageCollector &mygc::GarbageCollector::getCollector() {
  return sGarbageCollector;
}
void mygc::GarbageCollector::stopTheWorldLocked() {
  stop_the_world(sAttachedThreads);
}
void mygc::GarbageCollector::restartTheWorldLocked() {
  restart_the_world();
}
