//
// Created by liu on 19-10-30.
//

#ifndef MYGC_OBJECTRECORD_H
#define MYGC_OBJECTRECORD_H

#include <cstddef>
#include "IDescriptor.h"
#include "Tools.h"
namespace mygc {
constexpr uint64_t bits(const uint64_t x) {
  uint64_t i = 64;
  uint64_t mask = 0x8000000000000000;
  while (i > 0) {
    if ((mask & x) != 0) return i;
    i--;
    mask /= 2;
  }
  return 1;
}
class ObjectRecord {
 public:
  enum Location {
    kYoungGeneration,
    kOldGeneration,
    kLargeObjects,
    kEnd,
  };
  ObjectRecord() : mDescriptor(nullptr), copied(false) {
    nextNonTrivial = nullptr;
    preNonTrivial = nullptr;
    forwardAddress = nullptr;
  }
  IDescriptor *getDescriptor() {
    return mDescriptor;
  }
  void setDescriptor(IDescriptor *descriptor) {
    ObjectRecord::mDescriptor = descriptor;
  }
  char *getData() {
    return data;
  }
  bool isCopied() const {
    return copied;
  }
  void setCopied(bool copied) {
    ObjectRecord::copied = copied;
  }
  Location getLocation() {
    return location;
  }
  void setLocation(Location location) {
    ObjectRecord::location = location;
  }
  ObjectRecord *getNextNonTrivial() {
    return nextNonTrivial;
  }
  ObjectRecord *getPreNonTrivial() {
    return preNonTrivial;
  }
  ObjectRecord *getForwardAddress() {
    return forwardAddress;
  }
  void setNextNonTrivial(ObjectRecord *nextNonTrivial) {
    ObjectRecord::nextNonTrivial = nextNonTrivial;
  }
  void setPreNonTrivial(ObjectRecord *preNonTrivial) {
    ObjectRecord::preNonTrivial = preNonTrivial;
  }
  void setForwardAddress(ObjectRecord *forwardAddress) {
    ObjectRecord::forwardAddress = forwardAddress;
  }
 private:
  bool copied : 1;
  Location location : bits(static_cast<int>(Location::kEnd));
  IDescriptor *mDescriptor;
  union {
    struct {
      ObjectRecord *nextNonTrivial;
      ObjectRecord *preNonTrivial;
    };
    ObjectRecord *forwardAddress;
  };
  char data[];
};
}//namespace mygc
#endif //MYGC_OBJECTRECORD_H
