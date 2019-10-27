//
// Created by liu on 19-10-25.
//
#include "gtest/gtest.h"
#include "../headers/gc_ptr.h"

class Bar {
 public:
  int *i;
  mygc::gc_ptr<int> p4;
};

class Foo {
 public:
  int i;
  mygc::gc_ptr<int> p1;
  float f;
  mygc::gc_ptr<int> p2;
  double ds[16];
  mygc::gc_ptr<int> p3;
  Bar bar;
  Bar *pBar;
  mygc::gc_ptr<int> p5;
};

class CppWrapperTest : public testing::Test {};

TEST_F(CppWrapperTest, ptrMemberTest) {
  Foo foo;
  auto v1 = mygc::gc_ptr<Foo>::getGcPtrIndices(&foo);
  ASSERT_EQ(v1.size(), 5);
  std::vector<size_t> v2;
  v2.push_back((char *) &foo.p1 - (char *) &foo);
  v2.push_back((char *) &foo.p2 - (char *) &foo);
  v2.push_back((char *) &foo.p3 - (char *) &foo);
  v2.push_back((char *) &foo.bar.p4 - (char *) &foo);
  v2.push_back((char *) &foo.p5 - (char *) &foo);
  ASSERT_EQ(v1, v2);

}

