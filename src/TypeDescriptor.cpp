//
// Created by liu on 19-11-21.
//

#include "TypeDescriptor.h"
#include "ObjectRecord.h"
#include "Tools.h"
mygc::ITypeDescriptor::ITypeDescriptor(size_t typeSize, bool completed) : mTypeSize(typeSize), mCompleted(completed) {
  mBlockIndex = Tools::getLastOneFromRight(sizeof(OldRecord) + mTypeSize);
}
mygc::SingleType::SingleType(size_t typeSize,
                             std::vector<size_t> &&indices,
                             void (*destructor)(void *),
                             bool completed) : ITypeDescriptor(typeSize, completed),
                                               mIndices(std::move(indices)),
                                               mDestructor(destructor) {}
void mygc::SingleType::update(std::vector<size_t> &&indices, bool completed) {
  mIndices = std::move(indices);
  mCompleted = completed;
}
