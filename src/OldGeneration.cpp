//
// Created by liu on 19-11-12.
//

#include "OldGeneration.h"
#include "ObjectRecord.h"
#include "YoungGeneration.h"
#include "GcReference.h"
#include "Tools.h"
#include "GarbageCollector.h"

mygc::OldRecord *mygc::OldGeneration::copyFromYoungSTW(YoungRecord *from) {
  auto *descriptor = from->descriptor;
  // choose which block to use first
  auto blockIndex = descriptor->getBlockIndex();
  auto *block = mBlocks[blockIndex];
  auto pair = block->getUnusedAndMark([](size_t size) {
    if (GarbageCollector::getCollector().willOom(size)) {
      throw std::runtime_error("out of memory");
    } else {
      GarbageCollector::getCollector().updateTotalSize();
    }
  });
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
mygc::OldGeneration::OldGeneration() : mScavenger(&OldGeneration::scavenge, this), mTerminate(false) {
//  mScavenger.detach();
}
void mygc::OldGeneration::onScanEnd() {
  // first scan objects on mBlackList, these objects are unreachable objects but it's finalizer haven't be called yet
  std::unique_lock<std::mutex> lock(mBlackFinalizerMutex);
  auto *record = mBlackList.getHead();
  while (record) {
    mark(record);
    record = record->nonTrivialNode.next;
  }
  lock.unlock();
  // scan finished, all unknown objects are unreachable
  // but unreachable objects with finalizer still need to be marked, until the scavenger finished executing finalizer
  while (auto *grayRecord = mGrayList.getHead()) {
    mark(grayRecord);
    mGrayList.remove(grayRecord);
    lock.lock();
    mBlackList.add(grayRecord);
    lock.unlock();
  }
  // notify scavenger to work
  mCV.notify_one();
  for (auto &block : mBlocks) {
    if (block) {
      block->onScanEnd();
    }
  }
  // collect finished, all objects state is unknown now
  mGrayList = std::move(mWhiteList);
}
bool mygc::OldGeneration::mark(mygc::OldRecord *record) {
  auto blockIndex = record->descriptor->getBlockIndex();
  auto block = mBlocks[blockIndex];
  if (!block->isMarked(record->index)) {
    block->mark(record->index);
    return true;
  } else {
    return false;
  }
}
void mygc::OldGeneration::scavenge() {
  while (true) {
    std::unique_lock<std::mutex> lock(mBlackFinalizerMutex);
    auto *record = mBlackList.getHead();
    while (!record) {
      if (mTerminate) return;
      mCV.wait(lock);
      record = mBlackList.getHead();
    }
    lock.unlock();
    record->descriptor->callDestructor(record->data);
    lock.lock();
    mBlackList.remove(record);
    lock.unlock();
  }
}
mygc::OldGeneration::~OldGeneration() {
  std::unique_lock<std::mutex> lock(mBlackFinalizerMutex);
  mTerminate = true;
  lock.unlock();
  mCV.notify_all();
  for (auto &block : mBlocks) {
    delete block;
  }
  mScavenger.join();
}
void mygc::OldGeneration::scan(mygc::OldRecord *record) {
  if (mark(record)) {
    if (record->descriptor->nonTrivial()) {
      mGrayList.remove(record);
      mWhiteList.add(record);
    }
  }
}
size_t mygc::OldGeneration::getAllocatedSize() {
  size_t size = 0;
  for (auto &block : mBlocks) {
    if (block) {
      size += block->allocatedSize();
    }
  }
  return size;
}
