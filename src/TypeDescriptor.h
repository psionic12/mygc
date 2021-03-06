//
// Created by liu on 19-11-21.
//

#ifndef MYGC_TYPEDESCRIPTOR_H
#define MYGC_TYPEDESCRIPTOR_H
#include <utility>
#include <vector>
#include <map>
namespace mygc {
//no use, just for type safe;
class Object {};

class ITypeDescriptor {
 public:
  ITypeDescriptor(size_t typeSize, bool completed);
  virtual ~ITypeDescriptor() = default;
  size_t typeSize() {
    return mTypeSize;
  }
  /// get index of the block in old generation which this type belongs to, big object do not use this
  int getBlockIndex() {
    return mBlockIndex;
  }
  bool isCompleted() {
    return mCompleted;
  }
  virtual bool nonTrivial() = 0;
  virtual void callDestructor(Object *object) = 0;
 protected:
  const size_t mTypeSize;
  bool mCompleted;
  int mBlockIndex;
};
class SingleType : public ITypeDescriptor {
 public:
  SingleType(size_t typeSize,
             std::vector<size_t> &&indices,
             void (*destructor)(void *object),
             bool completed);
  std::vector<size_t> &getIndices() {
    return mIndices;
  }
  bool nonTrivial() override {
    return mDestructor != nullptr;
  }
  void callDestructor(Object *object) override {
    if (mDestructor) mDestructor(object);
  }
  void update(std::vector<size_t> &&indices, bool completed);
 private:
  void (*mDestructor)(void *object);
  std::vector<size_t> mIndices;
};
}//namespace mygc
#endif //MYGC_TYPEDESCRIPTOR_H
