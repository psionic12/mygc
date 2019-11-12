//
// Created by liu on 19-10-23.
//

#include <signal.h>
#include <glog/logging.h>
#include "GarbageCollector.h"

#include "GarbageCollector.h"
#include "stop_the_world.h"

#define MYGC_HEAP_SIZE (1 << 22)

#ifndef MYGC_STOP_SIGNAL
#define MYGC_STOP_SIGNAL SIGRTMIN + ('m' + 'y' + 'g' + 'c') % (SIGRTMAX - SIGRTMIN)
#endif

std::set<pthread_t> mygc::GarbageCollector::sAttachedThreads;

mygc::GarbageCollector::GarbageCollector()
    : mHeap(MYGC_HEAP_SIZE) {
  stop_the_world_init();
  // initial glog
//  google::InitGoogleLogging(nullptr);
  FLAGS_logtostderr = true;
}

mygc::ObjectRecord * mygc::GarbageCollector::New(size_t size) {
  std::lock_guard<std::mutex> guard(mGcMutex);
  return nullptr;
}
void mygc::GarbageCollector::collectLocked() {
}
bool mygc::GarbageCollector::inHeap(void *ptr) {
  std::lock_guard<std::mutex> guard(mGcMutex);
  return mHeap.inHeapLocked(ptr);
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
  sAttachedThreads.emplace(thread);
}
void mygc::GarbageCollector::detachThread(pthread_t thread) {
  std::lock_guard<std::mutex> guard(mGcMutex);
  sAttachedThreads.erase(thread);
}
mygc::GarbageCollector &mygc::GarbageCollector::getCollector() {
  static GarbageCollector garbageCollector;
  return garbageCollector;
}
void mygc::GarbageCollector::stopTheWorldLocked() {
  stop_the_world(sAttachedThreads);
}
void mygc::GarbageCollector::restartTheWorldLocked() {
  restart_the_world();
}
std::set<pthread_t> mygc::GarbageCollector::getAttachedThreads() {
  std::lock_guard<std::mutex> guard(mGcMutex);
  return sAttachedThreads;
}
