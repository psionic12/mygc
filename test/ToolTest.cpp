//
// Created by liu on 19-11-14.
//

#include <gtest/gtest.h>
#include "../src/Tools.h"

class ToolTest : public testing::Test {};

TEST_F(ToolTest, findFirst0) {
  size_t allOne = std::numeric_limits<size_t>::max();
  for (int i = 0; i < sizeof(size_t) * 8; i++) {
    auto n = allOne & ~(1l << i);
    ASSERT_EQ(mygc::Tools::getFirstZero(n), sizeof(size_t) * 8 - i - 1);
  }
  ASSERT_EQ(mygc::Tools::getFirstZero(0), 0);
}