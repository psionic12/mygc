//
// Created by liu on 19-11-12.
//

#ifndef MYGC_YOUNGGENERATION_H
#define MYGC_YOUNGGENERATION_H

#include <set>
#include "Heap.h"
#include "TypeDescriptor.h"
#include "ObjectRecord.h"
#include "RecordList.h"
namespace mygc {
class YoungGeneration {
 public:
  YoungGeneration();
  mygc::YoungRecord *allocate(ITypeDescriptor *descriptor);
  YoungNonTrivialList &getFinalizerList() {
    return mFinalizerList;
  }
  bool inHeapLocked(void *ptr);
 private:
  Heap mHeap;
  YoungNonTrivialList mFinalizerList;
};

}//namespace mygc
#endif //MYGC_YOUNGGENERATION_H
