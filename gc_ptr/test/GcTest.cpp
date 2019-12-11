//
// Created by liu on 19-12-9.
//
#include <gtest/gtest.h>
#include <mutex>
#include <thread>
#include "../headers/gc_ptr.h"
using namespace mygc;

class GCTest : public testing::Test {};
std::vector<int> v(128);

class Tester {
 public:
  Tester() {
    mConstructorCalled = true;
    std::unique_lock<std::mutex> lock(mMutex);
    static int i = 0;
    mId = i++;
    v[mId] += 1;
    lock.unlock();
//    DLOG(INFO) << "Tester: " << mId << "(" << this << ")" << std::endl;
  }
  ~Tester() {
    std::unique_lock<std::mutex> lock(mMutex);
    v[mId] -= 1;
    lock.unlock();
//    DLOG(INFO) << "~Tester: " << mId << "(" << this << ")" << std::endl;
  }
  bool constructorCalled() {
    return mConstructorCalled;
  }
  int getId() {
    return mId;
  }
 private:
  bool mConstructorCalled = false;
  std::mutex mMutex;
  char mPlaceHolder[1024];
  int mId;
};

void worker2() {
  DLOG(INFO) << "worker" << std::endl;
  gc_ptr<Tester> t;
  for (int i = 0; i < 10; i++) {
    t = make_gc<Tester>();
  }
  for (int i = 0; i < 10; i++) {
    t = make_gc<Tester>();
  }
  for (int i = 0; i < 10; i++) {
    t = make_gc<Tester>();
  }
  DLOG(INFO) << "t is " << t->getId() << std::endl;
}

TEST_F(GCTest, gcTest) {
  std::thread thread2(worker2);
  DLOG(INFO) << "start" << std::endl;
  gc_ptr<Tester> t;
  for (int i = 0; i < 10; i++) {
    t = make_gc<Tester>();
    ASSERT_EQ(t->constructorCalled(), true);
  }
  for (int i = 0; i < 10; i++) {
    t = make_gc<Tester>();
  }
  for (int i = 0; i < 10; i++) {
    t = make_gc<Tester>();
  }
  for (int i = 0; i < 10; i++) {
    t = make_gc<Tester>();
  }
  DLOG(INFO) << "t is " << t->getId() << std::endl;
  sleep(2);
  ASSERT_EQ(v, std::vector<int>(128));
  DLOG(INFO) << "test end" << std::endl;
  thread2.join();
}

