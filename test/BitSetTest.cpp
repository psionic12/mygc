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
  {
    std::vector<mygc::BitSet::ElementType> v;
    v.push_back(0x8000000000000000);
    mygc::BitSet bitSet;
    bitSet.setAll({0, 0}, {0, 0});
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
  std::vector<mygc::BitSet::ElementType> v1{0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF};
  ASSERT_EQ(bitSet.data(), v1);
  bitSet.clear();
  std::vector<mygc::BitSet::ElementType> v2{0, 0, 0};
  ASSERT_NE(bitSet.data(), v1);
  ASSERT_EQ(bitSet.data(), v2);
}

TEST_F(BitSetTest, singleBitTest) {
  mygc::BitSet bitSet;
  bitSet.safeSet({0, 0});
  ASSERT_EQ(bitSet.data(), std::vector<mygc::BitSet::ElementType>{0x8000000000000000});
  bitSet.set({0, 63});
  ASSERT_EQ(bitSet.data(), std::vector<mygc::BitSet::ElementType>{0x8000000000000001});
}

TEST_F(BitSetTest, indexTest) {
  mygc::BitSet bitSet;
  auto index = bitSet.getUnset().getIndex();
  ASSERT_EQ(index, 0);
  bitSet.safeSet({0, 0});
  index = bitSet.getUnset().getIndex();
  ASSERT_EQ(index, 1);
  bitSet.setAll({0, 0}, {1, 1});
  index = bitSet.getUnset().getIndex();
  ASSERT_EQ(index, 66);
}

TEST_F(BitSetTest, XORTest) {
  mygc::BitSet bitSet1;
  bitSet1.safeSet({1, 2});
  bitSet1.safeSet({1, 3});
  mygc::BitSet bitSet2;
  bitSet2.safeSet({1, 0});
  bitSet2.safeSet({1, 3});
  auto result = bitSet1.XOR(bitSet2);
  std::vector<mygc::BitSet::ElementType> v{0, 0xA000000000000000};
  ASSERT_EQ(result.data(), v);
}

TEST_F(BitSetTest, isSetTest) {
  mygc::BitSet bitSet;
  bitSet.safeSet({33, 62});
  ASSERT_EQ(bitSet.isSet(2174), true);
}