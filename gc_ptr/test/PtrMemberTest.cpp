//
// Created by liu on 19-10-25.
//
#include "gtest/gtest.h"
#include "../headers/gc_ptr.h"

class PtrMemberTest : public testing::Test {};

TEST_F(PtrMemberTest, correctnessTest) {
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

  };
  Foo foo;
  auto ptr = mygc::make_gc<Foo>();
  auto v1 = mygc::GcReference::getIndices(typeid(Foo).hash_code());
  ASSERT_EQ(v1.size(), 7);
  std::vector<size_t> v2;
  v2.push_back((char *) &foo.p1 - (char *) &foo);
  v2.push_back((char *) &foo.p2 - (char *) &foo);
  v2.push_back((char *) &foo.p3 - (char *) &foo);
  v2.push_back((char *) &foo.p4 - (char *) &foo);
  v2.push_back((char *) &foo.bar.p5 - (char *) &foo);
  v2.push_back((char *) &foo.p6 - (char *) &foo);
  v2.push_back((char *) &foo.p7 - (char *) &foo);

  ASSERT_EQ(v1, v2);
}

class Baz {

};
class Foo {

};

class Bar : public Foo {

};

template <typename T>
class A {};

TEST_F(PtrMemberTest, convertionTest) {
  mygc::gc_ptr<Foo> p(mygc::make_gc<Foo>());

  p = mygc::make_gc<Bar>();
  mygc::gc_ptr<A<int>> a = nullptr;
  auto p2 = mygc::make_gc<Foo>();
  auto p3 = mygc::make_gc<Bar>();

  mygc::gc_ptr<Foo[]> array(nullptr);
  array = nullptr;
  mygc::gc_ptr<Foo[]> a1(array);
//  a1 = mygc::make_gc<Bar*>();
//  a1 = mygc::make_gc<Bar[]>(3);
}
