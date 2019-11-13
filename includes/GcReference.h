//
// Created by liu on 19-10-30.
//

#ifndef MYGC_REFERENCE_H
#define MYGC_REFERENCE_H

#include "../src/GarbageCollector.h"

namespace mygc {
class ObjectRecord;
class GcReference {
 private:
  ObjectRecord *mPtr = nullptr;
 protected:
  GcReference(size_t size);
  virtual ~GcReference();
  void *getReference();
 public:
  ObjectRecord *getRecord();
  void update(ObjectRecord *newRecord);
};

}//namespace mygc

#endif //MYGC_REFERENCE_H
