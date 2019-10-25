//
// Created by liu on 19-10-23.
//

#include "Heap.h"
using namespace mygc;
void *Heap::allocate(size_t size) {
  std::lock_guard<std::mutex> guard(mMutex);
  if (mTail + size > mEnd) {
    return nullptr;
  } else {
    auto *ptr = mTail;
    mTail += size;
    return ptr;
  }
}
void Heap::clear() {
  std::lock_guard<std::mutex> guard(mMutex);
  mTail = mStart;
}
