//
// Created by liu on 19-10-23.
//

#include "Heap.h"
using namespace mygc;
void *Heap::allocateLocked(size_t size) {
  if (mTail + size > mEnd) {
    return nullptr;
  } else {
    auto *ptr = mTail;
    mTail += size;
    return ptr;
  }
}
void Heap::clearStopped() {
  mTail = mStart;
}
Heap::Heap(size_t size) : mSize(size), mStart(new char[size]), mTail(mStart), mEnd(mStart + size) {}
Heap::~Heap() {
  delete[] mStart;
}
bool Heap::inHeapLocked(void *ptr) {
  return ptr >= mStart && ptr < mEnd;
}
