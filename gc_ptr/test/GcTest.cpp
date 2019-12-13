//
// Created by liu on 19-12-9.
//
#include <gtest/gtest.h>
#include <mutex>
#include <thread>
#include "../headers/gc_ptr.h"
using namespace mygc;

class GCTest : public testing::Test {};

class Tester {
 public:
  Tester() {
//    DLOG(INFO) << "start construct";
    mConstructorCalled = true;
    std::unique_lock<std::mutex> lock(mMutex);
    static int i = 0;
    mId = i++;
    Tester::v1[mId] += 1;
    Tester::v2[mId] = mId;
    lock.unlock();

    DLOG(INFO) << "Tester: " << mId << "(" << this << ")" << std::endl;
//    DLOG(INFO) << "finished construct: " << mId << "(" << this << ")";
  }
  ~Tester() {
    std::unique_lock<std::mutex> lock(mMutex);
    Tester::v1[mId] -= 1;
    lock.unlock();
    DLOG(INFO) << "~Tester: " << mId << "(" << this << ")" << std::endl;
  }
  bool constructorCalled() {
    return mConstructorCalled;
  }
  int getId() {
    return mId;
  }
  gc_ptr<Tester> mChild;
  bool mConstructorCalled = false;
  char mPlaceHolder[512];
  int mId;
  static std::mutex mMutex;
  static std::vector<int> v1;
  static std::vector<int> v2;

};
std::mutex Tester::mMutex;
std::vector<int> Tester::Tester::v1;
std::vector<int> Tester::Tester::v2;
void worker2() {
  DLOG(INFO) << "worker" << std::endl;
  gc_ptr<Tester> t;
  for (int i = 0; i < 10; i++) {
    t = make_gc<Tester>();
    t->mChild = t;
  }
  for (int i = 0; i < 10; i++) {
    t = make_gc<Tester>();
  }
  for (int i = 0; i < 10; i++) {
    t = make_gc<Tester>();
  }
  DLOG(INFO) << "t is " << t->getId() << std::endl;
  t = nullptr;
  GcReference::collect();
}

TEST_F(GCTest, gcTest) {
  Tester::Tester::v1.resize(70);
  Tester::Tester::v2.resize(70);

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
  GcReference::collect();
  std::this_thread::sleep_for(std::chrono::seconds(3));
  for (int i = 0; i < Tester::v2.size(); i++) {
    ASSERT_EQ(Tester::v2[i], i) << " : Tester::v2[" << i << "] is " << Tester::v2[i] << std::endl;
  }
  for (int i = 0; i < Tester::v1.size(); i++) {
    ASSERT_TRUE(Tester::v1[i] == 0) << " : Tester::v1[" << i << "] is " << Tester::v1[i] << std::endl;
  }
  DLOG(INFO) << "test end" << std::endl;
}

TEST_F(GCTest, arrayTest) {
  auto &v1 = Tester::Tester::v1;
  auto &v2 = Tester::Tester::v2;
  v1.clear();
  v2.clear();
  v1.resize(10);
  v2.resize(10);
}

