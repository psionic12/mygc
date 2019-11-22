//
// Created by liu on 19-11-19.
//

#ifndef MYGC_BLOCK_H
#define MYGC_BLOCK_H
#include "DynamicSlots.h"
#include "BitSet.h"
namespace mygc {
class IBlock {
 public:
  virtual ~IBlock() = default;
  virtual void *getUnusedAndMark() = 0;
  virtual void onCollectionFinished() = 0;
  virtual void mark(size_t index) = 0;
};

template<size_t Size>
class Block : public IBlock {
 private:
  typedef char SlotType[Size];
  //current set is used to get definitely unused slot, it's "used" sets are not reliable, but "unused" sets are reliable
  BitSet *mCurrentSet;
  // new set is used to mark alive objects during gc
  BitSet *mNewSet;
  DynamicSlots<SlotType> mSlots;
 public:
  Block() : mCurrentSet(new BitSet), mNewSet(new BitSet) {}
  virtual ~Block() {
    delete (mCurrentSet);
    delete (mNewSet);
  }
  void *getUnusedAndMark() override {
    auto coordinate = mCurrentSet->getUnset();
    auto index = coordinate.getIndex();
    auto *ptr = mSlots.safeGetSlot(index);
    mCurrentSet->set(coordinate);
    return ptr;
  }
  void onCollectionFinished() override {
    // when collecting finished ,mNewSet got the full information about usage of this block
    // so we assign it to mCurrentSet for next collecting.
    auto *temp = mCurrentSet;
    mCurrentSet = mNewSet;
    // clean mNewSet for next collecting
    mNewSet = temp;
    mNewSet->clear();
  }
  void mark(size_t index) override {
    mCurrentSet->set(index);
  }
};
} //namespace mygc

#endif //MYGC_BLOCK_H
