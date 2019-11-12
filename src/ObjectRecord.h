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
  size_t size = 0;
  IDescriptor *descriptor = nullptr;
  ObjectRecord *nextNonTrivial = nullptr;
  ObjectRecord *preNonTrivial = nullptr;
  const char *data[];
};
}//namespace mygc
#endif //MYGC_OBJECTRECORD_H
