//
// Created by liu on 19-11-12.
//

#include "OldGeneration.h"
#include "ObjectRecord.h"

#ifndef HEAP_SIZE
#define HEAP_SIZE 2 << 22
#endif

mygc::ObjectRecord *mygc::OldGeneration::copyTo(ObjectRecord *from) {
  return nullptr;
}
mygc::OldGeneration::OldGeneration() {

}
