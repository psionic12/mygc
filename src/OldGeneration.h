//
// Created by liu on 19-11-12.
//

#ifndef MYGC_OLDGENERATION_H
#define MYGC_OLDGENERATION_H

#include "Heap.h"
namespace mygc {
class ObjectRecord;
class OldGeneration {
 public:
  OldGeneration();
  ObjectRecord *copyTo(ObjectRecord *from);
 private:
  Heap mHeap;
};
}

#endif //MYGC_OLDGENERATION_H
