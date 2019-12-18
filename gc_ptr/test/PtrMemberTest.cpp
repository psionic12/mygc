//
// Created by liu on 19-10-25.
//
#include <chrono>
#include <thread>
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
    mygc::gc_ptr<int[]> p8;
  };
  Foo foo;
  auto ptr = mygc::make_gc<Foo>();
  auto v1 = mygc::GcReference::getIndices(typeid(Foo).hash_code());
  ASSERT_EQ(v1.size(), 8);
  std::vector<size_t> v2;
  v2.push_back((char *) &foo.p1 - (char *) &foo);
  v2.push_back((char *) &foo.p2 - (char *) &foo);
  v2.push_back((char *) &foo.p3 - (char *) &foo);
  v2.push_back((char *) &foo.p4 - (char *) &foo);
  v2.push_back((char *) &foo.bar.p5 - (char *) &foo);
  v2.push_back((char *) &foo.p6 - (char *) &foo);
  v2.push_back((char *) &foo.p7 - (char *) &foo);
  v2.push_back((char *) &foo.p8 - (char *) &foo);
  ASSERT_EQ(v1, v2);
}



TEST_F(PtrMemberTest, arrayModifyTest) {
  mygc::gc_ptr<int[]> array = mygc::make_gc<int[]>(5);
  for (int i = 0; i < 5; i++) {
    array[i] = i;
  }
  for (int i = 0; i < 5; i++) {
    ASSERT_EQ(array[i], i);
  }
  mygc::GcReference::collect();
  for (int i = 0; i < 5; i++) {
    ASSERT_EQ(array[i], i);
  }
  for (int i = 4; i <= 0; i--) {
    array[i] = i;
  }
  for (int i = 4; i <= 0; i--) {
    ASSERT_EQ(array[i], i);
  }
  array = nullptr;
  mygc::GcReference::collect();
  std::this_thread::sleep_for(std::chrono::seconds(3));
  for (int i = 0; i < 5; i++) {
    ASSERT_EXIT((array[i] = 1, exit(0)), ::testing::KilledBySignal(SIGSEGV), ".*");
  }
}


TEST_F(PtrMemberTest, singleModifyTest) {
  mygc::gc_ptr<int> ptr = mygc::make_gc<int>(1);
  ASSERT_EQ(*ptr, 1);
  auto ptr2 = ptr;
  ASSERT_EQ(ptr == ptr2, true);
  mygc::GcReference::collect();
  ASSERT_EQ(*ptr, 1);
  ASSERT_EQ(ptr == ptr2, true);
  ASSERT_EQ(*ptr2, 1);
  ptr = nullptr;
  mygc::GcReference::collect();
  std::this_thread::sleep_for(std::chrono::seconds(3));
//  ASSERT_DEATH((*ptr, exit(0)), ".*");
}