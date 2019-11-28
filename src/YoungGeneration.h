//
// Created by liu on 19-11-12.
//

#ifndef MYGC_YOUNGGENERATION_H
#define MYGC_YOUNGGENERATION_H

#include <set>
#include "Heap.h"
#include "IDescriptor.h"
#include "TypeDescriptor.h"
#include "ObjectRecord.h"
#include "FinalizerList.h"
namespace mygc {
class YoungGeneration {
 public:
  YoungGeneration();
  mygc::YoungRecord * allocate(TypeDescriptor &descriptor);
  FinalizerList& getFinalizerList() {
    return mFinalizerList;
  }
 private:
  Heap mHeap;
  FinalizerList mFinalizerList;
};

}//namespace mygc
#endif //MYGC_YOUNGGENERATION_H
