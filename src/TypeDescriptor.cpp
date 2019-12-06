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
mygc::ArrayType::ArrayType(size_t typeSize, ITypeDescriptor *elementType, size_t counts)
    : ITypeDescriptor(typeSize, true), mElementType(elementType), mCounts(counts) {}
bool mygc::ArrayType::nonTrivial() {
  ITypeDescriptor *descriptor = mElementType;
  while (auto *arrayType = dynamic_cast<ArrayType *>(descriptor)) {
    descriptor = arrayType->mElementType;
  }
  auto *singleType = dynamic_cast<SingleType *>(descriptor);
  return singleType->nonTrivial();
}
void mygc::ArrayType::callDestructor(mygc::Object *object) {
  for (int i = 0; i < mCounts; i++) {
    mElementType->callDestructor(object + (i * mElementType->typeSize()));
  }
}
