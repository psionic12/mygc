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
struct NonTrivialNode {
  Record *pre = nullptr;
  Record *next = nullptr;
};

struct Record {
  Location location = Location::kYoungGeneration;
  TypeDescriptor *descriptor = nullptr;
  NonTrivialNode nonTrivialNode;
};

struct OldRecord : Record {
  size_t index = 0;
  Object data[];
};
class YoungGeneration;
struct YoungRecord : Record {
  bool copied = false;
  // if copied, this address is the new location in old generation
  OldRecord *forwardAddress = nullptr;
  YoungGeneration *generation = nullptr;
  Object data[];
};

class LargeRecord;
struct LargeRecordNode {
  LargeRecord *pre = nullptr;
  LargeRecord *next = nullptr;
};

struct LargeRecord : Record {
  LargeRecordNode largeRecordNode;
  Object data[];
};
}

#endif //MYGC_OBJECTRECORD2_H
