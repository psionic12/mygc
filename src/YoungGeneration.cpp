//
// Created by liu on 19-11-12.
//
#include "YoungGeneration.h"
#include "ObjectRecord.h"

#ifndef YOUNG_GENERATION_SIZE
#define YOUNG_GENERATION_SIZE 2 << 12
#endif
mygc::YoungGeneration::YoungGeneration() : mHeap(YOUNG_GENERATION_SIZE), mFinalizerHead(nullptr) {}
mygc::YoungRecord *mygc::YoungGeneration::allocate(mygc::TypeDescriptor &descriptor) {
  auto *record = (YoungRecord *) mHeap.allocate(sizeof(YoungRecord) + descriptor.typeSize());
  record->location = Location::kYoungGeneration;
  record->descriptor = &descriptor;
  record->copied = false;
  record->forwardAddress = nullptr;
  if (descriptor.nonTrivial()) {
    auto *temp = mFinalizerHead;
    mFinalizerHead = record;
    record->preNonTrivial = temp;
    if (record->preNonTrivial) {
      record->preNonTrivial->nextNonTrivial = record;
    }
    record->nextNonTrivial = nullptr;
  }
  return record;
}
mygc::YoungRecord *mygc::YoungGeneration::getFinalizerHeader() const {
  return mFinalizerHead;
}
