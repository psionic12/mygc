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

struct ObjectRecordHeader {
  Location location = Location::kYoungGeneration;
  TypeDescriptor *descriptor = nullptr;
};

struct YoungRecord : ObjectRecordHeader {
  bool copied = false;
  YoungRecord *nextNonTrivial = nullptr;
  YoungRecord *preNonTrivial = nullptr;
  YoungRecord *forwardAddress = nullptr;
  Object data[];
};

struct OldRecord : ObjectRecordHeader {
  Object data[];
};
}

#endif //MYGC_OBJECTRECORD2_H
