#include "gtest/gtest.h"
#include "../src/Heap.h"

class HeapTest : public testing::Test {};

TEST_F(HeapTest, heapTest) {
  mygc::Heap chunk(16);
  auto *ptr = chunk.allocateLocked(8);
  ASSERT_NE(ptr, nullptr);
  ptr = chunk.allocateLocked(4);
  ASSERT_NE(ptr, nullptr);
  ptr = chunk.allocateLocked(8);
  ASSERT_EQ(ptr, nullptr);
  chunk.clearLocked();
  ptr = chunk.allocateLocked(16);
  ASSERT_NE(ptr, nullptr);
  ptr = chunk.allocateLocked(1);
  ASSERT_EQ(ptr, nullptr);
}