//
// Created by liu on 19-10-30.
//

#ifndef MYGC_REFERENCE_H
#define MYGC_REFERENCE_H

#include <cstddef>
namespace mygc {
class ObjectRecordHeader;
class GcReference {
 private:
  ObjectRecordHeader *mPtr = nullptr;
 protected:
  GcReference(size_t typeId);
  virtual ~GcReference();
  void *getReference();
 public:
  ObjectRecordHeader *getRecord();
  void update(ObjectRecordHeader *newRecord);
};

}//namespace mygc

#endif //MYGC_REFERENCE_H
