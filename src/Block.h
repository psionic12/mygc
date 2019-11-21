//
// Created by liu on 19-11-19.
//

#ifndef MYGC_BLOCK_H
#define MYGC_BLOCK_H
#include "DynamicSlots.h"
#include "BitSet.h"
namespace mygc {
template<size_t Size>
class Block {
 private:
  typedef char SlotType[Size];
  BitSet mBitset;
  DynamicSlots<SlotType> mSlots;
 public:
  void *getUnused() {
    auto coordinate = mBitset.getUnset();
    auto index = coordinate.getIndex();
    auto *ptr = mSlots.safeGetSlot(index);
    mBitset.set(coordinate);
    return ptr;
  }
  void unsetMarks() {
    mBitset.clear();
  }
  void mark(size_t index) {
    mBitset.set(index);
  }
};
} //namespace mygc

#endif //MYGC_BLOCK_H
