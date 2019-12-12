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
    inline size_t getIndex() {
      return group * kElementSize + offset;
    }
  };
  typedef uint64_t ElementType;
  BitSet() = default;
  explicit BitSet(std::vector<ElementType> &&v);
  Coordinate getUnset();
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
  BitSet XOR(const BitSet &set);
  const std::vector<ElementType> &data() const;
  bool isSet(size_t index);
  void swap(BitSet& set);
  size_t bitCounts();
 private:
  static constexpr int kElementSize = sizeof(ElementType) * 8;
  static constexpr ElementType kMask = 1UL << (kElementSize - 1);
  std::vector<ElementType> mBitSet;
  inline ElementType &getOrCreate(size_t groupId) {
    if (mBitSet.size() <= groupId) {
      mBitSet.resize(groupId + 1);
    }
    return mBitSet[groupId];
  }
};
}//namespace mygc

#endif //MYGC_BITSET_H
