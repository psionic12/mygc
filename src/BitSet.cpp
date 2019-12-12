//
// Created by liu on 19-11-15.
//

#include <limits>
#include <cstring>
#include <assert.h>
#include "BitSet.h"
#include "Tools.h"
mygc::BitSet::Coordinate mygc::BitSet::getUnset() {
  size_t i = 0;
  do {
    // check if there's a zero in this number
    auto number = getOrCreate(i);
    if (number != std::numeric_limits<decltype(number)>::max()) {
      auto offset = Tools::getFirstZeroFromLeft(number);
      return {i, offset};
    }
    i++;
  } while (i < mBitSet.size());
  //ignore control may reach end warning, we will finally find one
}
void mygc::BitSet::setAll(const mygc::BitSet::Coordinate &from, const mygc::BitSet::Coordinate &to) {
  constexpr int kMaxIndex = sizeof(ElementType) * 8 - 1;
  assert(from.offset >= 0 && from.offset <= kMaxIndex && "out of scope");
  assert((to.offset >= 0 && to.offset <= kMaxIndex) && "out of scope");
  assert((from.group * kElementSize + from.offset <= to.group * kElementSize + to.offset) && "invalid range");
  auto &last = getOrCreate(to.group);
  last |= std::numeric_limits<ElementType>::max() << (kMaxIndex - to.offset);
  if (to.group - from.group > 1) {
    std::fill(mBitSet.begin() + (from.group + 1),
              mBitSet.begin() + (to.group),
              std::numeric_limits<ElementType>::max());
  }
  auto &first = mBitSet[from.group];

  if (from.group == to.group) {
    first &= std::numeric_limits<ElementType>::max() >> from.offset;
  } else {
    first |= std::numeric_limits<ElementType>::max() >> from.offset;
  }
}
const std::vector<mygc::BitSet::ElementType> &mygc::BitSet::data() const {
  return mBitSet;
}
void mygc::BitSet::safeSet(const mygc::BitSet::Coordinate &index) {
  getOrCreate(index.group);
  set(index);
}
void mygc::BitSet::safeSet(size_t index) {
  getOrCreate(index / kElementSize);
  set(index);
}
mygc::BitSet mygc::BitSet::XOR(const BitSet &set) {
  auto it1 = mBitSet.begin();
  auto it2 = set.data().begin();
  std::vector<mygc::BitSet::ElementType> v3;
  while (it2 != set.data().end()) {
    v3.push_back(*it1 xor *it2);
    it1++;
    it2++;
  }
  return BitSet(std::move(v3));
}
mygc::BitSet::BitSet(std::vector<mygc::BitSet::ElementType> &&v) : mBitSet(std::move(v)) {}
bool mygc::BitSet::isSet(size_t index) {
  return (mBitSet[index / kElementSize] & kMask >> index % kElementSize) != 0;
}
void mygc::BitSet::swap(mygc::BitSet &set) {
  mBitSet.swap(set.mBitSet);
}
size_t mygc::BitSet::bitCounts() {
  return mBitSet.size() * kElementSize;
}
