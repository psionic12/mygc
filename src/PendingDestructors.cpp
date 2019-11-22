//
// Created by liu on 19-11-13.
//

#include "PendingDestructors.h"
#include "ObjectRecord.h"
mygc::PendingDestructors::PendingDestructors() : mHead(new ObjectRecord), mTail(mHead) {
}
mygc::PendingDestructors::~PendingDestructors() {
  delete (mHead);
}
void mygc::PendingDestructors::add(mygc::ObjectRecord *record) {
// add it to non-trivial list
  auto *previous = mTail;
  record->setPreNonTrivial(previous);
  previous->setNextNonTrivial(record);
  mTail = record;
}
void mygc::PendingDestructors::tryRemove(mygc::ObjectRecord *record) {
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
void mygc::PendingDestructors::clear() {
  auto *ptr = mHead->getNextNonTrivial();
  while (ptr) {
    ptr->getDescriptor()->callDestructor(ptr->getData());
    ptr = ptr->getNextNonTrivial();
  }
  mHead->setNextNonTrivial(nullptr);
}
