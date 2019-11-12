//
// Created by liu on 19-11-12.
//

#include <stdexcept>
#include "YoungGeneration.h"
#include "ObjectRecord.h"
#include "OldGeneration.h"
#include "GcReference.h"
#include "IDescriptor.h"

#ifndef YOUNG_GENERATION_SIZE
#define YOUNG_GENERATION_SIZE 2 << 12
#endif
mygc::YoungGeneration::YoungGeneration(OldGeneration &oldGeneration, std::set<GcReference *> &gcRoot)
    : mHeap(YOUNG_GENERATION_SIZE),
      mNonTrivialHeader(new ObjectRecord),
      mNonTrivialTail(mNonTrivialHeader),
      mOldGeneration(oldGeneration),
      mGcRoot(gcRoot) {}
mygc::YoungGeneration::~YoungGeneration() {
  delete (mNonTrivialHeader);
}
void *mygc::YoungGeneration::allocateLocked(size_t size, IDescriptor *descriptor, bool nonTrivial) {
  auto *record = reinterpret_cast<ObjectRecord *>(mHeap.allocateLocked(sizeof(ObjectRecord) + size));
  if (record == nullptr) {
    collectLocked();
    record = reinterpret_cast<ObjectRecord *>(mHeap.allocateLocked(size));
    if (record == nullptr) {
      throw std::runtime_error("out of memory");
    }
  }
  record->size = size;
  record->descriptor = descriptor;
  if (nonTrivial) {
    // add it to non-trivial list
    record->preNonTrivial = mNonTrivialTail;
    mNonTrivialTail->nextNonTrivial = record;
    mNonTrivialTail = record;
  }
  return record->data;
}
void mygc::YoungGeneration::collectLocked() {
  for (auto *ref : mGcRoot) {
    auto *record = ref->getRecord();
    auto *ptr = markAndCopy(record);
    ref->update(ptr);
  }
}
mygc::ObjectRecord *mygc::YoungGeneration::markAndCopy(mygc::ObjectRecord *record) {
  auto *descriptor = record->descriptor;
  const auto &pair = descriptor->getIndices();
  auto group = pair.first;
  auto &indices = pair.second;
  ObjectRecord *currentRecord = record;
  size_t totalSize = sizeof(ObjectRecord) + record->size;
  // loop for arrays
  for (size_t i = 0; i < group; i++) {
    currentRecord = (ObjectRecord *) ((char *) currentRecord + totalSize);
    for (auto index : indices) {
      auto *ref = (GcReference *) (currentRecord->data + index);
      auto *newPtr = markAndCopy(ref->getRecord());
      ref->update(newPtr);
    }
  }

  // remove this record from non-trivial list
  if (record->preNonTrivial) {
    record->preNonTrivial->nextNonTrivial = record->nextNonTrivial;
  }

  return mOldGeneration.copyTo(record);
}
