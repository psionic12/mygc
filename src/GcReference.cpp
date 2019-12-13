//
// Created by liu on 19-10-30.
//

#include <GcReference.h>

#include "GcReference.h"
#include "GarbageCollector.h"

void mygc::GcReference::gcAlloca(size_t typeId) {
  auto &collector = mygc::GarbageCollector::getCollector();
  mPtr = collector.New(collector.getTypeById(typeId));
}
void mygc::GcReference::update(mygc::Record *newRecord) {
  mPtr = newRecord;
}
void *mygc::GcReference::getReference() {
  if (mPtr->location == Location::kYoungGeneration) {
    return ((YoungRecord *) mPtr)->data;
  } else if (mPtr->location == Location::kOldGeneration) {
    return ((OldRecord *) mPtr)->data;
  } else {
    return ((LargeRecord *) mPtr)->data;
  }
}
mygc::Record *mygc::GcReference::getRecord() {
  return mPtr;
}
void mygc::GcReference::registerType(size_t typeId,
                                     size_t typeSize,
                                     std::vector<size_t> &&indices,
                                     void (*destructor)(void *),
                                     bool completed = true) {
  auto &collector = mygc::GarbageCollector::getCollector();
  collector.registerType(typeId, typeSize, std::move(indices), destructor, completed);
}

void mygc::GcReference::registerType(size_t id, size_t typeSize, size_t elementType, size_t counts) {
  auto &collector = mygc::GarbageCollector::getCollector();
  collector.registerType(id, typeSize, elementType, counts);
}
bool mygc::GcReference::isCompletedDescriptor(size_t typeId) {
  auto &collector = mygc::GarbageCollector::getCollector();
  return collector.getTypeById(typeId)->isCompleted();
}
bool mygc::GcReference::isInYoungGeneration(void *ptr) {
  auto &collector = mygc::GarbageCollector::getCollector();
  return collector.inHeap(ptr);
}
void mygc::GcReference::addRoots(mygc::GcReference *ptr) {
  auto &collector = mygc::GarbageCollector::getCollector();
  collector.addRoots(ptr);
}
void mygc::GcReference::removeRoots(mygc::GcReference *ptr) {
  auto &collector = mygc::GarbageCollector::getCollector();
  collector.removeRoots(ptr);
}
void mygc::GcReference::attachThread(pthread_t thread) {
  auto &collector = mygc::GarbageCollector::getCollector();
  collector.attachThread(thread);
}
void mygc::GcReference::detachThread(pthread_t thread) {
  auto &collector = mygc::GarbageCollector::getCollector();
  collector.detachThread(thread);
}
std::vector<size_t> mygc::GcReference::getIndices(size_t typeId) {
  auto &collector = mygc::GarbageCollector::getCollector();
  return collector.getIndices(typeId);
}
std::set<pthread_t> mygc::GcReference::getAttachedThreads() {
  auto &collector = mygc::GarbageCollector::getCollector();
  return collector.getAttachedThreads();
}
std::set<mygc::GcReference *> mygc::GcReference::getRoots() {
  auto &collector = mygc::GarbageCollector::getCollector();
  return collector.getRoots();
}
void mygc::GcReference::collect() {
  auto &collector = mygc::GarbageCollector::getCollector();
  collector.collect();
}
