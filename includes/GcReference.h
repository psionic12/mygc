//
// Created by liu on 19-10-30.
//

#ifndef MYGC_REFERENCE_H
#define MYGC_REFERENCE_H

#include <cstddef>
namespace mygc {
class Record;
class GcReference {
 private:
  Record *mPtr = nullptr;
 protected:
  GcReference(size_t typeId);
  virtual ~GcReference();
  void *getReference();
 public:
  Record *getRecord();
  void update(Record *newRecord);
};

}//namespace mygc

#endif //MYGC_REFERENCE_H
