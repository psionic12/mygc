//
// Created by liu on 19-10-30.
//

#ifndef MYGC_OBJECTRECORD_H
#define MYGC_OBJECTRECORD_H

#include <cstddef>
#include "IDescriptor.h"
namespace mygc {
class ObjectRecord {
 public:
  ObjectRecord() : size(0), descriptor(nullptr), copied(false) {
    nextNonTrivial = nullptr;
    preNonTrivial = nullptr;
    forwardAddress = nullptr;
  }
  size_t size;
  IDescriptor *descriptor;
  bool copied;
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
