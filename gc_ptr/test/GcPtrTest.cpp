//
// Created by liu on 19-10-25.
//
#include "gtest/gtest.h"
#include "../headers/gc_ptr.h"

class GcPtrTest : public testing::Test {};

TEST_F(GcPtrTest, correctnessTest) {
  class Bar {
   public:
    int *i;
    mygc::gc_ptr<int> p5;
  };

  class Foo {
   public:
    int i;
    mygc::gc_ptr<double> p1;
    float f;
    mygc::gc_ptr<int> p2;
    double ds[16];
    mygc::gc_ptr<int> p3;
    mygc::gc_ptr<float> p4;
    Bar bar;
    Bar *pBar = new Bar;
    mygc::gc_ptr<Bar> p6;
    mygc::gc_ptr<mygc::gc_ptr<int>> p7;
//    mygc::gc_ptr<Bar[2]> p8;
  };
  Foo foo;
  auto ptr = mygc::make_gc<Foo>();
  auto v1 = mygc::GcReference::getIndices(typeid(Foo).hash_code()).second;
  ASSERT_EQ(v1.size(), 7);
  std::vector<size_t> v2;
  v2.push_back((char *) &foo.p1 - (char *) &foo);
  v2.push_back((char *) &foo.p2 - (char *) &foo);
  v2.push_back((char *) &foo.p3 - (char *) &foo);
  v2.push_back((char *) &foo.p4 - (char *) &foo);
  v2.push_back((char *) &foo.bar.p5 - (char *) &foo);
  v2.push_back((char *) &foo.p6 - (char *) &foo);
  v2.push_back((char *) &foo.p7 - (char *) &foo);
//  v2.push_back((char *) &foo.p8 - (char *) &foo);
  ASSERT_EQ(v1, v2);
}
