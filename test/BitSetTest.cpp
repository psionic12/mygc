//
// Created by liu on 19-11-15.
//

#include <gtest/gtest.h>
#include "../src/BitSet.h"

class BitSetTest : public testing::Test {};

TEST_F(BitSetTest, allSetTest) {
  {
    std::vector<mygc::BitSet::ElementType> v;
    v.push_back(0xFFFFFFFFFFFFFFFF);
    v.push_back(~(0xFFFFFFFFFFFFFFFF >> (4 + 1)));
    mygc::BitSet bitSet;
    bitSet.setAll({0, 0}, {1, 4});
    ASSERT_EQ(v, bitSet.data());
  }

  {
    std::vector<mygc::BitSet::ElementType> v;
    v.push_back(0xFFFFFFFFFFFFFFFF);
    v.push_back(~(0xFFFFFFFFFFFFFFFF >> (0 + 1)));
    mygc::BitSet bitSet;
    bitSet.setAll({0, 0}, {1, 0});
    ASSERT_EQ(v, bitSet.data());
  }
  {
    std::vector<mygc::BitSet::ElementType> v;
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0xFFFFFFFFFFFFFFFF >> 15);
    v.push_back(0xFFFFFFFFFFFFFFFF);
    v.push_back(0xFFFFFFFFFFFFFFFF);
    v.push_back(0xFFFFFFFFFFFFFFFF);
    v.push_back(~(0xFFFFFFFFFFFFFFFF >> (4 + 1)));

    mygc::BitSet bitSet;
    bitSet.setAll({3, 15}, {7, 4});
    ASSERT_EQ(v, bitSet.data());
  }
}

TEST_F(BitSetTest, emptyTest) {
  mygc::BitSet bitSet;
  ASSERT_EQ(bitSet.getUnset().group, 0);
  ASSERT_EQ(bitSet.getUnset().offset, 0);
}

TEST_F(BitSetTest, setTest) {
  mygc::BitSet bitSet;
  bitSet.setAll({0, 0}, {2, 15});
  ASSERT_EQ(bitSet.getUnset().group, 2);
  ASSERT_EQ(bitSet.getUnset().offset, 16);
  bitSet.safeSet({3, 24});
  ASSERT_EQ(bitSet.getUnset().group, 2);
  ASSERT_EQ(bitSet.getUnset().offset, 16);
  bitSet.setAll({0, 0}, {3, 24});
  ASSERT_EQ(bitSet.getUnset().group, 3);
  ASSERT_EQ(bitSet.getUnset().offset, 25);
  bitSet.unset({1, 63});
  ASSERT_EQ(bitSet.getUnset().group, 1);
  ASSERT_EQ(bitSet.getUnset().offset, 63);
}

TEST_F(BitSetTest, clearTest) {
  mygc::BitSet bitSet;
  bitSet.setAll({0, 0}, {2, 63});
  std::vector<mygc::BitSet::ElementType>
      v{0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF};
  ASSERT_EQ(bitSet.data(), v);
}

TEST_F(BitSetTest, singleBitTest) {
  mygc::BitSet bitSet;
  bitSet.safeSet({0, 0});
  ASSERT_EQ(bitSet.data(), std::vector<mygc::BitSet::ElementType>{0x8000000000000000});

}
