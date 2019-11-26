//
// Created by liu on 19-11-12.
//

#ifndef MYGC_YOUNGGENERATION_H
#define MYGC_YOUNGGENERATION_H

#include <set>
#include "Heap.h"
#include "IDescriptor.h"
#include "PendingDestructors.h"
#include "TypeDescriptor.h"
#include "ObjectRecord.h"
namespace mygc {

class ObjectRecord;
class OldGeneration;
class GcReference;

class YoungGeneration {
 public:
  YoungGeneration();
  mygc::YoungRecord * allocate(TypeDescriptor &descriptor);
  YoungRecord *getFinalizerHeader() const;
 private:
  Heap mHeap;
  YoungRecord* mFinalizerHead;
};

}//namespace mygc
#endif //MYGC_YOUNGGENERATION_H
