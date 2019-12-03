//
// Created by liu on 19-10-30.
//

#include <GcReference.h>

#include "GcReference.h"
#include "GarbageCollector.h"

mygc::GcReference::GcReference(size_t typeId) {
  auto &collector = mygc::GarbageCollector::getCollector();
  auto &descriptor = collector.getTypeById(typeId);
  mPtr = collector.New(descriptor);
}
void *mygc::GcReference::getReference() {
  if (mPtr->location == Location::kYoungGeneration) {
    return ((YoungRecord *) mPtr)->data;
  } else if (mPtr->location == Location::kOldGeneration) {
    return ((OldRecord *) mPtr)->data;
  } else {
    return nullptr;
  }
}
