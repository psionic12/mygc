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
      mOldGeneration(oldGeneration),
      mGcRoot(gcRoot) {}
void *mygc::YoungGeneration::allocateLocked(IDescriptor *descriptor, bool nonTrivial) {
  size_t size = descriptor->typeSize();
  auto *record = reinterpret_cast<ObjectRecord *>(mHeap.allocateLocked(sizeof(ObjectRecord) + size));
  if (record == nullptr) {
    collectStopped();
    record = reinterpret_cast<ObjectRecord *>(mHeap.allocateLocked(size));
    if (record == nullptr) {
      throw std::runtime_error("out of memory");
    }
  }
  record->setDescriptor(descriptor);
  if (nonTrivial) {
    mNonTrivialList.add(record);
  }
  return record->getData();
}
void mygc::YoungGeneration::collectStopped() {
  for (auto *ref : mGcRoot) {
    auto *record = ref->getRecord();
    auto *ptr = markAndCopyStopped(record);
    ref->update(ptr);
  }
  mHeap.clearStopped();
}
mygc::ObjectRecord *mygc::YoungGeneration::markAndCopyStopped(mygc::ObjectRecord *record) {
  if (!record->isCopied()) {
    // copy this survived object to old generation
    auto *forwardAddress = mOldGeneration.copyTo(record);
    record->setCopied(true);
    // use union, notice that remove record before set address
    mNonTrivialList.tryRemove(record);
    record->setForwardAddress(forwardAddress);
    // check children
    const auto &pair = record->getDescriptor()->getIndices();
    size_t group = pair.first; // big than 1 if it is an array
    const std::vector<size_t> &indices = pair.second;
    //OPT: most objects are not arrays
    for (int i = 0; i < group; i++) {
      auto *object = record->getData() + i;
      for (auto index : indices) {
        auto *ref = (GcReference *) (object + index);
        auto *childForwardAddress = markAndCopyStopped(ref->getRecord());
        ref->update(childForwardAddress);
      }
    }
  }
  return record->getForwardAddress();
}
