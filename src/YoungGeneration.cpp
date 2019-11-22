//
// Created by liu on 19-11-12.
//

#include <stdexcept>
#include "YoungGeneration.h"
#include "ObjectRecord.h"
#include "OldGeneration.h"
#include "GcReference.h"

#ifndef YOUNG_GENERATION_SIZE
#define YOUNG_GENERATION_SIZE 2 << 12
#endif
mygc::YoungGeneration::YoungGeneration(OldGeneration &oldGeneration, std::set<GcReference *> &gcRoot)
    : mHeap(YOUNG_GENERATION_SIZE),
      mOldGeneration(oldGeneration),
      mGcRoot(gcRoot) {}
mygc::ObjectRecord *mygc::YoungGeneration::allocateLocked(TypeDescriptor &descriptor) {
  auto *record = reinterpret_cast<ObjectRecord *>(mHeap.allocateLocked(descriptor.totalSize()));
  if (record == nullptr) {
    return nullptr;
  }
  record->setDescriptor(&descriptor);
  if (descriptor.nonTrivial()) {
    mPendingDestructors.add(record);
  }
  return record;
}
void mygc::YoungGeneration::collectStopped() {
  for (auto *ref : mGcRoot) {
    auto *record = ref->getRecord();
    if (record) {
      auto *ptr = markAndCopyStopped(record);
      ref->update(ptr);
    }
  }
}
mygc::ObjectRecord *mygc::YoungGeneration::markAndCopyStopped(mygc::ObjectRecord *record) {
  if (!record->isCopied()) {
    // copy this survived object to old generation
    auto *forward = mOldGeneration.copyToStopped(record);
    record->setCopied(true);
    // use union, notice that remove record before set address
    mPendingDestructors.tryRemove(record);
    record->setForwardAddress(forward);
    // do not use old record any more

    // check children
    const auto &pair = forward->getDescriptor()->getIndices();
    size_t group = pair.first; // big than 1 if it is an array
    const std::vector<size_t> &indices = pair.second;
    //OPT: most objects are not arrays
    for (int i = 0; i < group; i++) {
      auto *data = forward->getData() + i;
      for (auto index : indices) {
        auto *ref = (GcReference *) (data + index);
        auto *childForwardAddress = markAndCopyStopped(ref->getRecord());
        ref->update(childForwardAddress);
      }
    }
    return forward;
  } else {
    return record->getForwardAddress();
  }
}
void mygc::YoungGeneration::onCollectionFinished() {
  mPendingDestructors.clear();
  mHeap.clearStopped();
}
