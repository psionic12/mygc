//
// Created by liu on 19-12-9.
//
#include <gtest/gtest.h>
#include <mutex>
#include <thread>
#include "../headers/gc_ptr.h"
using namespace mygc;

class GCTest : public testing::Test {
 public:
  GCTest() {
//    google::InstallFailureSignalHandler();
  }
};

class Tester {
 public:
  Tester() {
    mConstructorCalled = true;
    std::unique_lock<std::mutex> lock(mMutex);
    static int i = 0;
    mId = i++;
    Tester::v1[mId] += 1;
    Tester::v2[mId] = mId;
    lock.unlock();

    DLOG(INFO) << "Tester: " << mId << "(" << this << ")" << std::endl;
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
  int mId;
  gc_ptr<Tester> mChild;
  bool mConstructorCalled = false;
  char mPlaceHolder[512];

  static std::mutex mMutex;
  static std::vector<int> v1;
  static std::vector<int> v2;

};
std::mutex Tester::mMutex;
std::vector<int> Tester::v1{};
std::vector<int> Tester::v2{};
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

TEST_F(GCTest, singleTest) {
  auto &v1 = Tester::v1;
  auto &v2 = Tester::v2;
  v1.clear();
  v2.clear();
  v1.resize(1);
  v2.resize(1);
  auto ptr = make_gc<Tester>();
  ASSERT_EQ(v2[0], 0) << " : v2[" << 0 << "] is " << v2[0] << std::endl;
  ASSERT_TRUE(v1[0] == 1) << " : v1[" << 0 << "] is " << v1[0] << std::endl;
  GcReference::collect();
  ASSERT_EQ(v2[0], 0) << " : v2[" << 0 << "] is " << v2[0] << std::endl;
  ASSERT_TRUE(v1[0] == 1) << " : v1[" << 0 << "] is " << v1[0] << std::endl;
  ptr = nullptr;
  GcReference::collect();
  std::this_thread::sleep_for(std::chrono::seconds(3));
  ASSERT_EQ(v2[0], 0) << " : v2[" << 0 << "] is " << v2[0] << std::endl;
  ASSERT_TRUE(v1[0] == 0) << " : v1[" << 0 << "] is " << v1[0] << std::endl;
}

TEST_F(GCTest, gcTest) {
  auto &v1 = Tester::v1;
  auto &v2 = Tester::v2;
  v1.clear();
  v2.clear();
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
  GcReference::collect();
  std::this_thread::sleep_for(std::chrono::seconds(3));
  for (int i = 0; i < v2.size(); i++) {
    ASSERT_EQ(v2[i], i) << " : v2[" << i << "] is " << v2[i] << std::endl;
  }
  for (int i = 0; i < v1.size(); i++) {
    ASSERT_TRUE(v1[i] == 0) << " : v1[" << i << "] is " << v1[i] << std::endl;
  }
  DLOG(INFO) << "test end" << std::endl;
}

TEST_F(GCTest, arrayOldTest) {
  auto &v1 = Tester::v1;
  auto &v2 = Tester::v2;
  v1.clear();
  v2.clear();
  v1.resize(5);
  v2.resize(5);
  gc_ptr<Tester[]> ptr = make_gc<Tester[]>(5);
  for (int i = 0; i < v2.size(); i++) {
    ASSERT_EQ(v2[i], i) << " : v2[" << i << "] is " << v2[i] << std::endl;
  }
  for (int i = 0; i < v1.size(); i++) {
    ASSERT_TRUE(v1[i] == 1) << " : v1[" << i << "] is " << v1[i] << std::endl;
  }
  GcReference::collect();
  for (int i = 0; i < v2.size(); i++) {
    ASSERT_EQ(v2[i], i) << " : v2[" << i << "] is " << v2[i] << std::endl;
  }
  for (int i = 0; i < v1.size(); i++) {
    ASSERT_TRUE(v1[i] == 1) << " : v1[" << i << "] is " << v1[i] << std::endl;
  }
  ptr = nullptr;
  GcReference::collect();
  std::this_thread::sleep_for(std::chrono::seconds(3));
  for (int i = 0; i < v2.size(); i++) {
    ASSERT_EQ(v2[i], i) << " : v2[" << i << "] is " << v2[i] << std::endl;
  }
  for (int i = 0; i < v1.size(); i++) {
    ASSERT_TRUE(v1[i] == 0) << " : v1[" << i << "] is " << v1[i] << std::endl;
  }
  DLOG(INFO) << "test end" << std::endl;
}

TEST_F(GCTest, arrayYoungTest) {
  auto &v1 = Tester::v1;
  auto &v2 = Tester::v2;
  v1.clear();
  v2.clear();
  v1.resize(5);
  v2.resize(5);
  gc_ptr<Tester[]> ptr = make_gc<Tester[]>(5);
  for (int i = 0; i < v2.size(); i++) {
    ASSERT_EQ(v2[i], i) << " : v2[" << i << "] is " << v2[i] << std::endl;
  }
  for (int i = 0; i < v1.size(); i++) {
    ASSERT_TRUE(v1[i] == 1) << " : v1[" << i << "] is " << v1[i] << std::endl;
  }
  ptr = nullptr;
  GcReference::collect();
  std::this_thread::sleep_for(std::chrono::seconds(3));
  for (int i = 0; i < v2.size(); i++) {
    ASSERT_EQ(v2[i], i) << " : v2[" << i << "] is " << v2[i] << std::endl;
  }
  for (int i = 0; i < v1.size(); i++) {
    ASSERT_TRUE(v1[i] == 0) << " : v1[" << i << "] is " << v1[i] << std::endl;
  }
  DLOG(INFO) << "test end" << std::endl;
}

