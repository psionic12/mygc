//
// Created by liu on 19-10-30.
//

#ifndef MYGC_REFERENCE_H
#define MYGC_REFERENCE_H

#include "GarbageCollector.h"

namespace mygc {

class GcReference {
 private:
  void *mPtr = nullptr;
 protected:
  GcReference(size_t size) {
    mPtr = GarbageCollector::getCollector().New(size);
  }
  virtual ~GcReference() {
    mPtr = nullptr;
  }
  void *getReference() {
    return mPtr;
  }
};

}//namespace mygc

#endif //MYGC_REFERENCE_H
