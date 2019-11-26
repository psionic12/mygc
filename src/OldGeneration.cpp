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
  void *ptr = block->getUnusedAndMark();
  auto *record = (OldRecord *) ptr;
  record->location = Location::kOldGeneration;
  record->descriptor = from->descriptor;
  memcpy(record->data, from->data, descriptor->typeSize());
  return record;
}
void mygc::OldGeneration::onCollectionFinished() {
  for (auto &block : mBlocks) {
    if (block) {
      block->onCollectionFinished();
    }
  }
}
