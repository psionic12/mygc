//
// Created by liu on 19-11-28.
//

#ifndef MYGC_FINALIZERLIST_H
#define MYGC_FINALIZERLIST_H
#include "ObjectRecord.h"
namespace mygc {
class FinalizerList {
 public:
  FinalizerList() = default;
  FinalizerList(const FinalizerList &) = delete;
  FinalizerList(FinalizerList &) = delete;
  FinalizerList(FinalizerList &&list) noexcept {
    this->head = list.head;
    list.head = nullptr;
  }
  FinalizerList &operator=(const FinalizerList &) = delete;
  FinalizerList &operator=(FinalizerList &) = delete;
  FinalizerList &operator=(FinalizerList &&list) noexcept {
    this->head = list.head;
    list.head = nullptr;
    return *this;
  };
  Record *getHead() {
    return head;
  }
  void add(Record *record) {
    Record *temp = head;
    head = record;
    record->preNonTrivial = nullptr;
    record->nextNonTrivial = temp;
    if (temp) {
      temp->preNonTrivial = record;
    }
  }
  void remove(Record *record) {
    auto *pre = record->preNonTrivial;
    auto *next = record->nextNonTrivial;
    if (pre) {
      pre->nextNonTrivial = record->nextNonTrivial;
    } else {
      head = record->nextNonTrivial;
    }
    if (next) {
      next->preNonTrivial = record->preNonTrivial;
    }
    record->preNonTrivial = nullptr;
    record->nextNonTrivial = nullptr;
  }
 private:
  Record *head = nullptr;
};
}//namespace mygc
#endif //MYGC_FINALIZERLIST_H
