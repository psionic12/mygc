//
// Created by liu on 19-11-13.
//

#include "NonTrivialList.h"
#include "ObjectRecord.h"
mygc::NonTrivialList::NonTrivialList() : mHead(new ObjectRecord), mTail(mHead) {
}
mygc::NonTrivialList::~NonTrivialList() {
  delete (mHead);
}
void mygc::NonTrivialList::add(mygc::ObjectRecord *record) {
// add it to non-trivial list
  auto *previous = mTail;
  record->preNonTrivial = previous;
  previous->nextNonTrivial = record;
  mTail = record;
}
void mygc::NonTrivialList::tryRemove(mygc::ObjectRecord *record) {
  auto *previous = record->preNonTrivial;
  auto *next = record->nextNonTrivial;
  if (previous) {
    previous->nextNonTrivial = next;
    if (next) {
      next->preNonTrivial = previous;
    } else {
      mTail = previous;
    }
  }
}
void mygc::NonTrivialList::clear() {
  auto *ptr = mHead->nextNonTrivial;
  while (ptr) {
    ptr->descriptor->destructor(ptr->data);
    ptr = ptr->nextNonTrivial;
  }
  mHead->nextNonTrivial = nullptr;
}
