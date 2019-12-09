//
// Created by liu on 19-10-23.
//
#include <glog/logging.h>

#include "Heap.h"
using namespace mygc;
void *Heap::allocate(size_t size) {
  if (mTail + size > mEnd) {
    return nullptr;
  } else {
    auto *ptr = mTail;
    mTail += size;
//    DLOG(INFO) << size << "allocated, " << (mEnd + 1 - mTail) * 100 / (float) (mEnd + 1 - mStart)
//    << "% (" << mEnd + 1 - mTail
//               << ") remained";
    return ptr;
  }
}
void Heap::reset() {
  mTail = mStart;
}
Heap::Heap(size_t size) : mSize(size), mStart(new char[size]), mTail(mStart), mEnd(mStart + size) {}
Heap::~Heap() {
  delete[] mStart;
}
bool Heap::inHeapLocked(void *ptr) {
  return ptr >= mStart && ptr < mEnd;
}
