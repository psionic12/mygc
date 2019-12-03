//
// Created by liu on 19-12-2.
//

#include "LargeObjects.h"
mygc::LargeRecord *mygc::LargeObjects::allocate(mygc::TypeDescriptor &descriptor) {
  char *ptr = new char[sizeof(LargeRecord) + descriptor.typeSize()];
  auto *largeRecord = (LargeRecord *) ptr;
  largeRecord->location = Location::kLargeObjects;
  largeRecord->descriptor = &descriptor;



}
