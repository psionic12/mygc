//
// Created by liu on 19-11-18.
//

#include <cstring>
#include <gtest/gtest.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include "../src/DynamicSlots.h"

class DynamicSlotsTest : public testing::Test {};

class SlotCounter {
 public:
  SlotCounter() : mUuid(sUuid) {
    sCounts++;
  }
  boost::uuids::uuid mUuid;
  static size_t sCounts;
  static boost::uuids::uuid sUuid;

};
size_t SlotCounter::sCounts = 0;
boost::uuids::uuid SlotCounter::sUuid = boost::uuids::random_generator()();

TEST_F(DynamicSlotsTest, countsTest) {
  mygc::DynamicSlots<SlotCounter> slots;
  slots.safeGetSlot(0);
  ASSERT_EQ(slots.data().size(), 1);
  ASSERT_EQ(SlotCounter::sCounts, 1);
  slots.safeGetSlot(10);
  ASSERT_EQ(slots.data().size(), 4);
  ASSERT_EQ(SlotCounter::sCounts, 15);
}

TEST_F(DynamicSlotsTest, boundaryTest) {
  mygc::DynamicSlots<SlotCounter> slots;
  auto *p = (SlotCounter *) slots.safeGetSlot(0);
  ASSERT_EQ(std::memcmp(&SlotCounter::sUuid, &p->mUuid, sizeof(boost::uuids::uuid)), 0);
//  ASSERT_NE(std::memcmp(&SlotCounter::sUuid, &(p - 1)->mUuid, sizeof(boost::uuids::uuid)), 0);
//  ASSERT_NE(std::memcmp(&SlotCounter::sUuid, &(p + 1)->mUuid, sizeof(boost::uuids::uuid)), 0);
  // mess up with the memory
  std::unique_ptr<double[4]> p1;
  std::unique_ptr<int[4]> p2;
  std::unique_ptr<char[4]> p3;
  std::unique_ptr<float[4]> p4;
  std::unique_ptr<long long[4]> p5;

  p = (SlotCounter *) slots.safeGetSlot(10);
//  ASSERT_NE(std::memcmp(&SlotCounter::sUuid, &(p - 4)->mUuid, sizeof(boost::uuids::uuid)), 0);
  ASSERT_EQ(std::memcmp(&SlotCounter::sUuid, &(p - 3)->mUuid, sizeof(boost::uuids::uuid)), 0);
  ASSERT_EQ(std::memcmp(&SlotCounter::sUuid, &(p - 2)->mUuid, sizeof(boost::uuids::uuid)), 0);
  ASSERT_EQ(std::memcmp(&SlotCounter::sUuid, &(p - 1)->mUuid, sizeof(boost::uuids::uuid)), 0);

  ASSERT_EQ(std::memcmp(&SlotCounter::sUuid, &p->mUuid, sizeof(boost::uuids::uuid)), 0);

  ASSERT_EQ(std::memcmp(&SlotCounter::sUuid, &(p + 1)->mUuid, sizeof(boost::uuids::uuid)), 0);
  ASSERT_EQ(std::memcmp(&SlotCounter::sUuid, &(p + 2)->mUuid, sizeof(boost::uuids::uuid)), 0);
  ASSERT_EQ(std::memcmp(&SlotCounter::sUuid, &(p + 3)->mUuid, sizeof(boost::uuids::uuid)), 0);
  ASSERT_EQ(std::memcmp(&SlotCounter::sUuid, &(p + 4)->mUuid, sizeof(boost::uuids::uuid)), 0);
//  ASSERT_NE(std::memcmp(&SlotCounter::sUuid, &(p + 5)->mUuid, sizeof(boost::uuids::uuid)), 0);
}