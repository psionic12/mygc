//
// Created by liu on 19-11-12.
//

#ifndef MYGC_YOUNGGENERATION_H
#define MYGC_YOUNGGENERATION_H

#include <set>
#include "Heap.h"
#include "IDescriptor.h"
#include "NonTrivialList.h"
namespace mygc {

class ObjectRecord;
class OldGeneration;
class GcReference;

class YoungGeneration {
 public:
  YoungGeneration(OldGeneration &oldGeneration, std::set<GcReference *> &gcRoot);
  void *allocateLocked(size_t size, IDescriptor *descriptor, bool nonTrivial);
  void collectStopped();
 private:
  Heap mHeap;
  NonTrivialList mNonTrivialList;
  OldGeneration &mOldGeneration;
  const std::set<GcReference *> &mGcRoot;
  ObjectRecord *markAndCopyStopped(ObjectRecord *record);

};

}//namespace mygc
#endif //MYGC_YOUNGGENERATION_H
