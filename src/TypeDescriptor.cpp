//
// Created by liu on 19-11-21.
//

#include "TypeDescriptor.h"
#include "ObjectRecord.h"
#include "Tools.h"
mygc::TypeDescriptor::TypeDescriptor(size_t typeSize,
                                     std::pair<const size_t, const std::vector<size_t>> &&indices,
                                     void (*destructor)(void *))
    : mTypeSize(typeSize), mIndices(std::move(indices)), mDestructor(destructor) {
  mBlockIndex = Tools::getLastOneFromRight(totalSize());
}
size_t mygc::TypeDescriptor::totalSize() {
  return sizeof(ObjectRecord) + mTypeSize;
}
