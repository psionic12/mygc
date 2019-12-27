//
// Created by liu on 19-11-28.
//

#ifndef MYGC_FINALIZERLIST_H
#define MYGC_FINALIZERLIST_H
#include "ObjectRecord.h"
namespace mygc {
template<typename RecordType, typename ListType, ListType RecordType::* pList>
class RecordList {
 public:
  RecordList() {}
  RecordList(const RecordList &) = delete;
  RecordList(RecordList &) = delete;
  RecordList(RecordList &&list) noexcept : RecordList() {
    this->head = list.head;
    list.head = nullptr;
  }
  RecordList &operator=(const RecordList &) = delete;
  RecordList &operator=(RecordList &) = delete;
  RecordList &operator=(RecordList &&list) noexcept {
    this->head = list.head;
    list.head = nullptr;
    return *this;
  };
  RecordType *getHead() {
    return head;
  }
  void reset() {
    head = nullptr;
  }
  void add(RecordType *record) {
    RecordType *temp = head;
    head = record;
    (record->*pList).pre = nullptr;
    (record->*pList).next = temp;
    if (temp) {
      (temp->*pList).pre = record;
    }
  }
  void remove(RecordType *record) {
    auto *pre = (record->*pList).pre;
    auto *next = (record->*pList).next;
    if (pre) {
      (pre->*pList).next = (record->*pList).next;
    } else {
      if (head == record) {
        head = (record->*pList).next;
      }
    }
    if (next) {
      (next->*pList).pre = (record->*pList).pre;
    }
    (record->*pList).pre = nullptr;
    (record->*pList).next = nullptr;
  }
 private:
  RecordType *head = nullptr;
};

typedef RecordList<YoungRecord, NonTrivialNode < YoungRecord>, &YoungRecord::nonTrivialNode>
YoungNonTrivialList;
typedef RecordList<OldRecord, NonTrivialNode < OldRecord>, &OldRecord::nonTrivialNode>
OldNonTrivialList;
typedef RecordList<LargeRecord, LargeListNode, &LargeRecord::largeListNode> LargeObjectList;
}//namespace mygc
#endif //MYGC_FINALIZERLIST_H
