//
// Created by liu on 19-12-9.
//
#include <gtest/gtest.h>
#include <mutex>
#include <thread>
#include "../headers/gc_ptr.h"
using namespace mygc;

class GCTest : public testing::Test {};
std::mutex mMutex;
std::vector<int> v1;
std::vector<int> v2;

class Tester {
 public:
  Tester() {
//    DLOG(INFO) << "start construct";
    mConstructorCalled = true;
    std::unique_lock<std::mutex> lock(mMutex);
    static int i = 0;
    mId = i++;
    v1[mId] += 1;
    v2[mId] = mId;
    lock.unlock();

//    DLOG(INFO) << "Tester: " << mId << "(" << this << ")" << std::endl;
//    DLOG(INFO) << "finished construct: " << mId << "(" << this << ")";
  }
  ~Tester() {
    std::unique_lock<std::mutex> lock(mMutex);
    v1[mId] -= 1;
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
  v1.resize(70);
  v2.resize(70);

  std::thread thread2(worker2);
  thread2.detach();
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
  t = nullptr;
  std::this_thread::sleep_for(std::chrono::seconds(3));
  for (int i = 0; i < v2.size(); i++) {
    ASSERT_EQ(v2[i], i) << " : v2[" << i << "] is " << v2[i] << std::endl;
  }
  for (int i = 0; i < v1.size(); i++) {
    ASSERT_TRUE(v1[i] == 1 || v1[i] == 0) << " : v1[" << i << "] is " << v1[i] << std::endl;
  }
  DLOG(INFO) << "test end" << std::endl;
}

