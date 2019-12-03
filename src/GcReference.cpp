//
// Created by liu on 19-10-30.
//

#include <GcReference.h>

#include "GcReference.h"
#include "GarbageCollector.h"

mygc::GcReference::GcReference(size_t typeId) : GcReference() {
  auto &collector = mygc::GarbageCollector::getCollector();
  auto &descriptor = collector.getTypeById(typeId);
  mPtr = collector.New(descriptor);
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
void mygc::GcReference::update(mygc::Record *newRecord) {
  mPtr = newRecord;
}
mygc::Record *mygc::GcReference::getRecord() {
  return mPtr;
}
mygc::GcReference::GcReference() {
  auto &collector = mygc::GarbageCollector::getCollector();
  if (isRoot()) {
    collector.addRoots(this);
  }
}
mygc::GcReference::~GcReference() {
  mPtr = nullptr;
  auto &collector = mygc::GarbageCollector::getCollector();
  if (isRoot()) {
    collector.removeRoots(this);
  }
}
bool mygc::GcReference::isRoot() {
  auto &collector = mygc::GarbageCollector::getCollector();
  return !collector.inHeap(this);
}
void mygc::GcReference::registeredType(size_t typeId,
                                       size_t typeSize,
                                       std::pair<const size_t, const std::vector<size_t>> &&indices,
                                       void (*destructor)(void *)) {
  auto &collector = mygc::GarbageCollector::getCollector();
  collector.registeredType(typeId, typeSize, std::move(indices), destructor);
}
bool mygc::GcReference::isRegistered(size_t typeId) {
  auto &collector = mygc::GarbageCollector::getCollector();
  try {
    collector.getTypeById(typeId);
    return true;
  } catch (const std::out_of_range &) {
    return false;
  }
}
