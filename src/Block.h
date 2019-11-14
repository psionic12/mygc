//
// Created by liu on 19-11-13.
//

#ifndef MYGC_BLOCK_H
#define MYGC_BLOCK_H

#include <cstddef>
#include <vector>
#include <limits>
#include <cmath>
#include "Tools.h"
namespace mygc {
template<size_t SLOT_SIZE>
class Block {
 public:
  void insertObject() {

  }
 private:
  typedef char SlotType[SLOT_SIZE];
  std::vector<SlotType *> mSlots;
  std::vector<uint64_t> mBitMaps;
  size_t getNextIndexAndMarked() {
    for (size_t i = 0; i < mBitMaps.size(); i++) {
      // check if there's a zero in this number
      auto number = mBitMaps[i];
      if (number != std::numeric_limits<decltype(number)>::max()) {
        auto offset = Tools::getFirstZero(number);
        auto index = i * sizeof(decltype(number)) * 8 + offset;
        mBitMaps |= 1ul << offset;
        return index;
      } else {
        continue;
      }
    }
  }
  SlotType *getNextSlot(uint64_t index) {
    uint64_t leadingBitPosition = Tools::getFirstOne(index);
    uint64_t offset = index & ~(1ul << leadingBitPosition);
    if (!mSlots[leadingBitPosition]) {
      // (int)(std::pow(2, leadingBitPosition - 1) + 0.5) generates 1,1,2,4,8... which doubles the former
      mSlots[leadingBitPosition] = new SlotType[std::pow(2, leadingBitPosition - 1) + 0.5];
    }
    return mSlots[leadingBitPosition] + offset;
  }
};
}//namespace mygc
#endif //MYGC_BLOCK_H
