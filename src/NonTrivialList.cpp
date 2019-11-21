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
  record->setPreNonTrivial(previous);
  previous->setNextNonTrivial(record);
  mTail = record;
}
void mygc::NonTrivialList::tryRemove(mygc::ObjectRecord *record) {
  auto *previous = record->getPreNonTrivial();
  auto *next = record->getNextNonTrivial();
  if (previous) {
    previous->setNextNonTrivial(next);
    if (next) {
      next->setPreNonTrivial(previous);
    } else {
      mTail = previous;
    }
  }
}
void mygc::NonTrivialList::clear() {
  auto *ptr = mHead->getNextNonTrivial();
  while (ptr) {
    ptr->getDescriptor()->destructor(ptr->getData());
    ptr = ptr->getNextNonTrivial();
  }
  mHead->setNextNonTrivial(nullptr);
}
