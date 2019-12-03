//
// Created by liu on 19-11-26.
//

#ifndef MYGC_OBJECTRECORD2_H
#define MYGC_OBJECTRECORD2_H

#include "TypeDescriptor.h"
namespace mygc {
enum class Location {
  kYoungGeneration,
  kOldGeneration,
  kLargeObjects,
};
class Record;
template<typename RecordType>
struct NonTrivialNode {
  RecordType *pre = nullptr;
  RecordType *next = nullptr;
};

struct Record {
  Location location = Location::kYoungGeneration;
  TypeDescriptor *descriptor = nullptr;
};

struct OldRecord : Record {
  NonTrivialNode<OldRecord> nonTrivialNode;
  size_t index = 0;
  Object data[];
};
class YoungGeneration;
struct YoungRecord : Record {
  NonTrivialNode<YoungRecord> nonTrivialNode;
  bool copied = false;
  // if copied, this address is the new location in old generation
  OldRecord *forwardAddress = nullptr;
  YoungGeneration *generation = nullptr;
  Object data[];
};

class LargeRecord;
struct LargeListNode {
  LargeRecord *pre = nullptr;
  LargeRecord *next = nullptr;
};

struct LargeRecord : Record {
  LargeListNode largeListNode;
  Object data[];
};
}

#endif //MYGC_OBJECTRECORD2_H
