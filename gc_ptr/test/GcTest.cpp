//
// Created by liu on 19-12-9.
//
#include <gtest/gtest.h>
#include "../headers/gc_ptr.h"
using namespace mygc;

class GCTest : public testing::Test {};

class Tester {
 public:
  Tester() {
    mConstructorCalled = true;
    static int i = 0;
    mId = i++;
//    DLOG(INFO) << "Tester: " << i << "(" << this << ")" << std::endl;
  }
  ~Tester() {
    DLOG(INFO) << "~Tester: " << mId << std::endl;
  }
  bool constructorCalled() {
    return mConstructorCalled;
  }
  int getId() {
    return mId;
  }
 private:
  bool mConstructorCalled = false;
  char mPlaceHolder[512];
  int mId;
};

TEST_F(GCTest, gcTest) {
  gc_ptr<Tester> t;
  for (int i = 0; i < 20; i++) {
    t = make_gc<Tester>();
    ASSERT_EQ(t->constructorCalled(), true);
  }
  for (int i = 0; i < 20; i++) {
    t = make_gc<Tester>();
  }
  for (int i = 0; i < 20; i++) {
    t = make_gc<Tester>();
  }


}

