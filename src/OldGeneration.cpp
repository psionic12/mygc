//
// Created by liu on 19-11-12.
//

#include "OldGeneration.h"
#include "ObjectRecord.h"

#ifndef HEAP_SIZE
#define HEAP_SIZE 2 << 22
#endif

mygc::OldRecord *mygc::OldGeneration::copyToStopped(YoungRecord *from) {
  auto *descriptor = from->descriptor;
  // choose which block to use first
  auto blockIndex = descriptor->getBlockIndex();
  auto *block = mBlocks[blockIndex];
  auto pair = block->getUnusedAndMark();
  void *ptr = pair.second;
  auto *record = (OldRecord *) ptr;
  record->index = pair.first;
  record->location = Location::kOldGeneration;
  record->descriptor = from->descriptor;
  memcpy(record->data, from->data, descriptor->typeSize());
  if (record->descriptor->nonTrivial()) {
    auto *temp = mLivingFinalizer;
    mLivingFinalizer = record;
    record->preNonTrivial = temp;
    if (record->preNonTrivial) {
      record->preNonTrivial->nextNonTrivial = record;
    }
    record->nextNonTrivial = nullptr;
  }
  return record;
}
mygc::OldGeneration::OldGeneration() : mLivingFinalizer(nullptr), mDeadFinalizer(nullptr) {}
void mygc::OldGeneration::onScanBegin() {

}
