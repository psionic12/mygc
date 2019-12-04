//
// Created by liu on 19-12-4.
//

#include "YoungGenerations.h"
mygc::YoungGeneration *mygc::YoungGenerations::getMine() {
  static thread_local std::thread::id myId = std::this_thread::get_id();
  if (mAttachedYoungGenerations[myId] == nullptr) {
    mAttachedYoungGenerations[myId] = mYoungPool.getCleanGeneration();
  }
  return mAttachedYoungGenerations[myId].get();
}
void mygc::YoungGenerations::onScanEnd() {
  for (auto &pair : mAttachedYoungGenerations) {
    mYoungPool.putDirtyGeneration(std::move(pair.second));
    pair.second = mYoungPool.getCleanGeneration();
  }
}
