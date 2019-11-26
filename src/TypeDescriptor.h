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
class TypeDescriptor {
 public:
  TypeDescriptor(size_t typeSize,
                 std::pair<const size_t, const std::vector<size_t>> &&indices,
                 void (*destructor)(void *object) = nullptr);
  /// get positions of gc references in this type, pair.first > 1 means this is an array type
  std::pair<const size_t, const std::vector<size_t>> &getIndices() {
    return mIndices;
  }
  void *callDestructor(Object *object) {
    mDestructor(object);
  }
  size_t typeSize() {
    return mTypeSize;
  }
  /// get index of the block in old generation which this type belongs to, big object do not use this
  int getBlockIndex() {
    return mBlockIndex;
  }
  bool nonTrivial() {
    return mDestructor == nullptr;
  }
 private:
  std::pair<const size_t, const std::vector<size_t>> mIndices;
  void (*mDestructor)(void *object);
  size_t mTypeSize;
  int mBlockIndex;
};
}//namespace mygc
#endif //MYGC_TYPEDESCRIPTOR_H
