//
// Created by liu on 19-11-26.
//

#include "YoungGenerationPool.h"
void mygc::YoungGenerationPool::finalize() {
  while (true) {
    std::unique_ptr<YoungGeneration> generation = nullptr;
    std::unique_lock<std::mutex> lock(mMutex);
    if (mDirty.empty()) {
      mCV.wait(lock);
    }
    generation = std::move(mDirty.back());
    mDirty.pop_back();
    lock.unlock();

    auto *ptr = generation->getFinalizerHeader();
    while (ptr) {
      ptr->descriptor->callDestructor(ptr->data);
      ptr = ptr->nextNonTrivial;
    }

    lock.lock();
    mClean.push_back(std::move(generation));
    lock.unlock();
  }
}
mygc::YoungGenerationPool::YoungGenerationPool() : mFinalizer(&YoungGenerationPool::finalize) {
  mFinalizer.detach();
}
void mygc::YoungGenerationPool::putDirtyGeneration(std::unique_ptr<mygc::YoungGeneration> &&generation) {
  std::unique_lock<std::mutex> lock(mMutex);
  mDirty.push_back(std::move(generation));
  lock.unlock();
  mCV.notify_one();
}
std::unique_ptr<mygc::YoungGeneration> mygc::YoungGenerationPool::getCleanGeneration() {
  std::unique_lock<std::mutex> lock(mMutex);
  if (!mClean.empty()) {
    auto generation = std::move(mClean.back());
    mClean.pop_back();
    return generation;
  } else {
    return {};
  }
}
