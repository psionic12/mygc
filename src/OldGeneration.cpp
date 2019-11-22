//
// Created by liu on 19-11-12.
//

#include "OldGeneration.h"
#include "ObjectRecord.h"

#ifndef HEAP_SIZE
#define HEAP_SIZE 2 << 22
#endif

mygc::ObjectRecord *mygc::OldGeneration::copyToStopped(ObjectRecord *from) {
  auto *descriptor = from->getDescriptor();
  // choose which block to use first
  auto blockIndex = descriptor->getBlockIndex();
  auto *block = mBlocks[blockIndex];
  void *ptr = block->getUnusedAndMark();
  auto *record = (ObjectRecord *) ptr;
  memcpy(record, from, descriptor->totalSize());
  record->setLocation(ObjectRecord::Location::kOldGeneration);
  if (descriptor->nonTrivial()) {
    mPendingDestructors.add(record);
  }
  return record;
}
void mygc::OldGeneration::onCollectionFinished() {
  mPendingDestructors.clear();
  for (auto &block : mBlocks) {
    if (block) {
      block->onCollectionFinished();
    }
  }
}
