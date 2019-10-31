//
// Created by liu on 19-10-23.
//

#include <signal.h>
#include <GarbageCollector.h>

#include "GarbageCollector.h"

#define MYGC_HEAP_SIZE (1 << 22)
#define STOP_SIGNAL SIGRTMIN + ('m' + 'y' + 'g' + 'c') % (SIGRTMAX - SIGRTMIN)

mygc::GarbageCollector mygc::GarbageCollector::sGarbageCollector;
std::mutex  mygc::GarbageCollector::mThreadBlocker;
std::condition_variable mygc::GarbageCollector::mBlockerCondition;

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
mygc::GarbageCollector::GarbageCollector()
    : mHeap(MYGC_HEAP_SIZE) {
  //TODO use sigact()
  signal(STOP_SIGNAL, GarbageCollector::stopHandler);
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
void mygc::GarbageCollector::stopTheWorldLocked() {
  auto self = pthread_self();
  for (auto thread : mAttachedThreads) {
    if (thread != self) {
      pthread_kill(thread, STOP_SIGNAL);
    }
  }
}
void mygc::GarbageCollector::restartTheWorldLocked() {
  mBlockerCondition.notify_all();
}
void mygc::GarbageCollector::attachThead(pthread_t thread) {
  mAttachedThreads.emplace(thread);
}
void mygc::GarbageCollector::detachThead(pthread_t thread) {
  mAttachedThreads.erase(thread);
}
void mygc::GarbageCollector::stopHandler(int signal) {
  std::unique_lock<std::mutex> lk(mThreadBlocker);
  mBlockerCondition.wait(lk);
}
mygc::GarbageCollector &mygc::GarbageCollector::getCollector() {
  return sGarbageCollector;
}
