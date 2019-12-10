//
// Created by liu on 19-11-26.
//
#include <glog/logging.h>
#include "YoungGenerationPool.h"
#include "Tester.h"
void mygc::YoungGenerationPool::scavenge() {
  while (true) {
    std::unique_ptr<YoungGeneration> generation = nullptr;
    std::unique_lock<std::mutex> lock(mMutex);
    while (mDirty.empty()) {
      if (mTerminate) {
        return;
      };
      mCV.wait(lock);
    }
    generation = std::move(mDirty.back());
    mDirty.pop_back();
    lock.unlock();

    auto *ptr = generation->getFinalizerList().getHead();
    while (ptr) {
      DLOG(INFO) << "young: call destructor on: " << ((Tester *) ptr->data)->mId << std::endl;
      ptr->descriptor->callDestructor(ptr->data);
      ptr = ptr->nonTrivialNode.next;
    }
    generation->reset();
    lock.lock();
    mClean.push_back(std::move(generation));
    lock.unlock();
  }
}
mygc::YoungGenerationPool::YoungGenerationPool() : mScavenger(&YoungGenerationPool::scavenge, this), mTerminate(false) {
//  mScavenger.detach();
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
    return std::make_unique<YoungGeneration>();
  }
}
mygc::YoungGenerationPool::~YoungGenerationPool() {
  std::unique_lock<std::mutex> lock(mMutex);
  mTerminate = true;
  lock.unlock();
  mCV.notify_all();
  mScavenger.join();
  DLOG(INFO) << "~YoungGenerationPool" << std::endl;
}
