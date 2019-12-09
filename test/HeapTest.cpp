#include "gtest/gtest.h"
#include "../src/Heap.h"

class HeapTest : public testing::Test {};

TEST_F(HeapTest, heapTest) {
  mygc::Heap chunk(16);
  auto *ptr = chunk.allocate(8);
  ASSERT_NE(ptr, nullptr);
  ptr = chunk.allocate(4);
  ASSERT_NE(ptr, nullptr);
  ptr = chunk.allocate(8);
  ASSERT_EQ(ptr, nullptr);
  chunk.reset();
  ptr = chunk.allocate(16);
  ASSERT_NE(ptr, nullptr);
  ptr = chunk.allocate(1);
  ASSERT_EQ(ptr, nullptr);
}