//
// Created by liu on 19-11-15.
//

#ifndef MYGC_BITSET_H
#define MYGC_BITSET_H

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <cstring>
namespace mygc {
class BitSet {
 public:
  struct Coordinate {
    size_t group;
    unsigned int offset;
  };
  typedef uint64_t ElementType;
  Coordinate getUnset();
  inline size_t getIndex(const Coordinate &pair) {
    return pair.group * kElementSize + pair.offset;
  }
  inline void set(const Coordinate &index) {
    mBitSet[index.group] |= kMask >> index.offset;
  }
  inline void set(size_t index) {
    mBitSet[index / kElementSize] |= kMask >> index % kElementSize;
  }
  inline void unset(const Coordinate &index) {
    mBitSet[index.group] &= ~(kMask >> index.offset);
  }
  inline void unset(size_t index) {
    mBitSet[index / kElementSize] &= ~(kMask >> index % kElementSize);
  }
  void safeSet(const Coordinate &index);
  void safeSet(size_t index);
  inline void clear() {
    memset(mBitSet.data(), 0, mBitSet.capacity() * sizeof(ElementType));
  }
  void setAll(const Coordinate &from, const Coordinate &to);
  const std::vector<ElementType> &data();
 private:
  static constexpr int kElementSize = sizeof(ElementType) * 8;
  static constexpr ElementType kMask = 1UL << (kElementSize - 1);
  std::vector<ElementType> mBitSet;
  inline ElementType &getOrCreate(size_t index) {
    if (mBitSet.size() <= index) {
      mBitSet.resize(index + 1);
    }
    return mBitSet[index];
  }
};
}//namespace mygc

#endif //MYGC_BITSET_H