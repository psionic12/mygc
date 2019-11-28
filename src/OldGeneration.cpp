//
// Created by liu on 19-11-12.
//

#include "OldGeneration.h"
#include "ObjectRecord.h"
#include "YoungGeneration.h"
#include "GcReference.h"

mygc::OldRecord *mygc::OldGeneration::copyFromYoungSTW(YoungRecord *from) {
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
    mWhiteList.add(record);
  }
  return record;
}
mygc::OldGeneration::OldGeneration() {}
void mygc::OldGeneration::onScanEnd() {
  // first scan objects on mBlackList, these objects are unreachable objects but it's finalizer haven't be called yet
  std::unique_lock<std::mutex> lock(mBlackFinalizerMutex);
  auto *record = (OldRecord *) mBlackList.getHead();
  while (record) {
    mark(record);
  }
  lock.unlock();
  // scan finished, all unknown objects are unreachable
  // but unreachable objects with finalizer still need to be marked, until the scavenger finished executing finalizer
  while (auto *grayRecord = (OldRecord *) mGrayList.getHead()) {
    mark(grayRecord);
    mGrayList.remove(grayRecord);
    lock.lock();
    mBlackList.add(grayRecord);
    lock.unlock();
  }
  for (auto &block : mBlocks) {
    block->onScanEnd();
  }
  // collect finished, all objects state is unknown now
  mGrayList = std::move(mWhiteList);
}
void mygc::OldGeneration::mark(mygc::OldRecord *record) {
  auto blockIndex = record->descriptor->getBlockIndex();
  auto block = mBlocks[blockIndex];
  if (!block->isMarked(record->index)) {
    block->mark(record->index);
    if (record->descriptor->nonTrivial()) {
      mGrayList.remove(record);
      mWhiteList.add(record);
    }
  }
}
