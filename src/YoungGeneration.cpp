//
// Created by liu on 19-11-12.
//
#include "YoungGeneration.h"
#include "ObjectRecord.h"
#include "OldGeneration.h"

mygc::YoungGeneration::YoungGeneration() : mHeap(OldGeneration::getMaxBlockSize()) {}
mygc::YoungRecord *mygc::YoungGeneration::allocate(ITypeDescriptor *descriptor, size_t counts) {
  auto *record = (YoungRecord *) mHeap.allocate(sizeof(YoungRecord) + descriptor->typeSize() * counts);
  if (!record) return nullptr;
  record->location = Location::kYoungGeneration;
  record->descriptor = descriptor;
  record->counts = counts;
  record->copied = false;
  record->forwardAddress = nullptr;
  record->generation = this;
  if (descriptor->nonTrivial()) {
    mFinalizerList.add(record);
  }
  return record;
}
bool mygc::YoungGeneration::inHeapLocked(void *ptr) {
  return mHeap.inHeapLocked(ptr);
}
size_t mygc::YoungGeneration::defaultSize() {
  return OldGeneration::getMaxBlockSize();
}
void mygc::YoungGeneration::reset() {
  mHeap.reset();
  mFinalizerList.reset();
}
