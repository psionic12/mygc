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
  virtual std::pair<size_t, void *> getUnusedAndMark(const std::function<void(size_t)> &f = {}) = 0;
  virtual void mark(size_t index) = 0;
  virtual bool isMarked(size_t index) = 0;
  virtual void onScanEnd() = 0;
  virtual size_t allocatedSize() = 0;
};

template<size_t Size>
class Block : public IBlock {
 private:
  typedef char SlotType[Size];
  // copying happens at scanning time, the old generation need to find available room for objects copied from young, so
// use a single set and set all to 0 before scanning is not acceptable.
  BitSet mNewSet;
  //current set is used to get definitely unused slot, it's "used" sets are not reliable, but "unused" sets are reliable
  BitSet mCurrentSet;
  DynamicSlots<SlotType> mSlots;
 public:
  std::pair<size_t, void *> getUnusedAndMark(const std::function<void(size_t)> &f) override {
    auto coordinate = mCurrentSet.getUnset();
    auto index = coordinate.getIndex();
    auto *ptr = mSlots.safeGetSlot(index, f);
    mCurrentSet.safeSet(coordinate);
    mNewSet.safeSet(coordinate);
    return {index, ptr};
  }
  bool isMarked(size_t index) override {
    return /*mCurrentSet.isSet(index) &&*/ mNewSet.isSet(index);
  }
  void mark(size_t index) override {
    mCurrentSet.safeSet(index);
    mNewSet.safeSet(index);
  }
  void onScanEnd() override {
    // now new set is fully reliable, make it as current set;
    mCurrentSet.swap(mNewSet);
    mNewSet.clear();
  }
  size_t allocatedSize() override {
    return mCurrentSet.bitCounts() * sizeof(SlotType);
  }
};
} //namespace mygc

#endif //MYGC_BLOCK_H
