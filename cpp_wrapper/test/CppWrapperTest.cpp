//
// Created by liu on 19-10-25.
//
#include "gtest/gtest.h"
#include "../headers/gc_ptr.h"

class CppWrapperTest : public testing::Test {};

TEST_F(CppWrapperTest, ptrMemberTest) {
  mygc::gc_ptr<int> foo;
  auto v1 = mygc::gc_ptr<mygc::gc_ptr<int>>::getGcPtrIndices(&foo);

  for (size_t i : v1) {
    std::cout << i << std::endl;
  }
}

