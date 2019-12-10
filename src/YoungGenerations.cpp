//
// Created by liu on 19-12-4.
//

#include "YoungGenerations.h"
mygc::YoungGeneration *mygc::YoungGenerations::getMine() {
  static thread_local pthread_t myId = pthread_self();
  if (mAttachedYoungGenerations[myId] == nullptr) {
    mAttachedYoungGenerations[myId] = mYoungPool.getCleanGeneration();
  }
  return mAttachedYoungGenerations[myId].get();
}
void mygc::YoungGenerations::onScanEnd() {
  for (auto &pair : mAttachedYoungGenerations) {
    if (pair.second) {
      if (pair.second->getFinalizerList().getHead()) {
        mYoungPool.putDirtyGeneration(std::move(pair.second));
      } else {
        pair.second->reset();
      }
    }
  }
}
