//
// Created by liu on 19-12-2.
//

#include "LargeObjects.h"
mygc::LargeRecord *mygc::LargeObjects::allocate(mygc::ITypeDescriptor *descriptor, size_t counts) {
  void *ptr = malloc(sizeof(LargeRecord) + descriptor->typeSize());
  auto *largeRecord = (LargeRecord *) ptr;
  largeRecord->location = Location::kLargeObjects;
  largeRecord->descriptor = descriptor;
  largeRecord->counts = counts;
  mGrayList.add(largeRecord);
  return largeRecord;
}
mygc::LargeObjects::LargeObjects() : mScavenger(&LargeObjects::scavenge, this), mTerminate(false) {}
void mygc::LargeObjects::scavenge() {
  while (true) {
    std::unique_lock<std::mutex> lock(mBlackListMutex);
    auto *record = mBlackList.getHead();
    while (!record) {
      if (mTerminate) return;
      mCV.wait(lock);
      record = mBlackList.getHead();
    }
    lock.unlock();
    if (record->descriptor->nonTrivial()) {
      for (int i = 0; i < record->counts; i++) {
        record->descriptor->callDestructor(record->data + i * record->descriptor->typeSize());
      }
    }
    free(record);
    lock.lock();
    mBlackList.remove(record);
    lock.unlock();
  }
}
void mygc::LargeObjects::onScanEnd() {
  auto *record = mGrayList.getHead();
  //TODO refactor RecordList to insert the whole chain
  while (record) {
    mGrayList.remove(record);
    std::unique_lock<std::mutex> lock(mBlackListMutex);
    mBlackList.add(record);
  }
}
void mygc::LargeObjects::mark(mygc::LargeRecord *record) {
  mGrayList.remove(record);
  mWhiteList.add(record);
}
mygc::LargeObjects::~LargeObjects() {
  std::unique_lock<std::mutex> lock(mBlackListMutex);
  mTerminate = true;
  lock.unlock();
  mCV.notify_all();
  mScavenger.join();
}
