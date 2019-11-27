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

struct Record {
  Location location = Location::kYoungGeneration;
  TypeDescriptor *descriptor = nullptr;
  Record *nextNonTrivial = nullptr;
  Record *preNonTrivial = nullptr;
};

struct YoungRecord : Record {
  bool copied = false;
  YoungRecord *forwardAddress = nullptr;
  Object data[];
};

struct OldRecord : Record {
  size_t index = 0;
  Object data[];
};
}

#endif //MYGC_OBJECTRECORD2_H
