//
// Created by liu on 19-11-12.
//

#ifndef MYGC_YOUNGGENERATION_H
#define MYGC_YOUNGGENERATION_H

#include <set>
#include "Heap.h"
#include "IDescriptor.h"
namespace mygc {

class ObjectRecord;
class OldGeneration;
class GcReference;

class YoungGeneration {
 public:
  YoungGeneration(OldGeneration &oldGeneration, std::set<GcReference *> &gcRoot);
  ~YoungGeneration();
  void *allocateLocked(size_t size, IDescriptor *descriptor, bool nonTrivial);
  void collectLocked();
 private:
  // link list header/tail for objects which has a non-trivial destructer
  ObjectRecord *mNonTrivialHeader;
  ObjectRecord *mNonTrivialTail;
  Heap mHeap;
  OldGeneration &mOldGeneration;
  const std::set<GcReference *> &mGcRoot;
  ObjectRecord *markAndCopy(ObjectRecord *record);

};

}//namespace mygc
#endif //MYGC_YOUNGGENERATION_H
