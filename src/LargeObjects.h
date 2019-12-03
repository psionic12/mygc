//
// Created by liu on 19-12-2.
//

#ifndef MYGC_LARGEOBJECTS_H
#define MYGC_LARGEOBJECTS_H

#include "ObjectRecord.h"
namespace mygc {

class LargeObjects {
 public:
  LargeRecord *allocate(TypeDescriptor &descriptor);
 private:
  LargeRecord *mHead = nullptr;
};

}//namespace mygc

#endif //MYGC_LARGEOBJECTS_H
