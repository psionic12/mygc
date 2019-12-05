//
// Created by liu on 19-11-21.
//

#include "TypeDescriptor.h"
#include "ObjectRecord.h"
#include "Tools.h"
mygc::TypeDescriptor::TypeDescriptor(size_t typeSize,
                                     std::pair<size_t, std::vector<size_t>> &&indices,
                                     void (*destructor)(void *),
                                     bool completed)
    : mTypeSize(typeSize), mIndices(std::move(indices)), mDestructor(destructor), mCompleted(completed) {
  mBlockIndex = Tools::getLastOneFromRight(sizeof(OldRecord) + mTypeSize);
}
