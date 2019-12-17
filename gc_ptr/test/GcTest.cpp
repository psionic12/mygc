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
    //TODO figure out how to make these cases isolated
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
    Tester::aliveness[mId] += 1;
    Tester::createdIndex[mId] = mId;
    lock.unlock();

//    DLOG(INFO) << "Tester: " << mId << "(" << this << ")" << std::endl;
  }
  ~Tester() {
    std::unique_lock<std::mutex> lock(mMutex);
    Tester::aliveness[mId] -= 1;
    lock.unlock();
//    DLOG(INFO) << "~Tester: " << mId << "(" << this << ")" << std::endl;
  }
  bool constructorCalled() {
    return mConstructorCalled;
  }
  int getId() {
    return mId;
  }
  static void assertAllCreated() {
    for (int i = 0; i < createdIndex.size(); i++) {
      ASSERT_EQ(createdIndex[i], i) << " : v2[" << i << "] is " << createdIndex[i] << std::endl;
    }
  }
  static void assertAllAlive() {
    for (int i = 0; i < aliveness.size(); i++) {
      ASSERT_TRUE(aliveness[i] == 1) << " : v1[" << i << "] is " << aliveness[i] << std::endl;
    }
  }
  static void assertAllDead() {
    for (int i = 0; i < aliveness.size(); i++) {
      ASSERT_TRUE(aliveness[i] == 0) << " : v1[" << i << "] is " << aliveness[i] << std::endl;
    }
  }
  static void reset(unsigned long size) {
    aliveness.clear();
    createdIndex.clear();
    aliveness.resize(size);
    createdIndex.resize(size);
  }
  int mId;
  gc_ptr<Tester> mChild;
  bool mConstructorCalled = false;
  char mPlaceHolder[512];

  static std::mutex mMutex;
  static std::vector<int> aliveness;
  static std::vector<int> createdIndex;

};
std::mutex Tester::mMutex;
std::vector<int> Tester::aliveness{};
std::vector<int> Tester::createdIndex{};
void worker2() {
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
  t = nullptr;
  GcReference::collect();
}

TEST_F(GCTest, singleTest) {
  auto &v1 = Tester::aliveness;
  auto &v2 = Tester::createdIndex;
  Tester::reset(1);
  auto ptr = make_gc<Tester>();
  Tester::assertAllCreated();
  Tester::assertAllAlive();
  GcReference::collect();
  Tester::assertAllAlive();
  ptr = nullptr;
  GcReference::collect();
  std::this_thread::sleep_for(std::chrono::seconds(3));
  Tester::assertAllDead();
}

TEST_F(GCTest, gcTest) {
  Tester::reset(70);
  std::thread thread2(worker2);
  thread2.detach();
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
  t = nullptr;
  GcReference::collect();
  std::this_thread::sleep_for(std::chrono::seconds(3));
  Tester::assertAllCreated();
  Tester::assertAllDead();
}

TEST_F(GCTest, arrayOldTest) {
  Tester::reset(5);
  gc_ptr<Tester[]> ptr = make_gc<Tester[]>(5);
  Tester::assertAllCreated();
  Tester::assertAllAlive();
  GcReference::collect();
  Tester::assertAllAlive();
  ptr = nullptr;
  GcReference::collect();
  std::this_thread::sleep_for(std::chrono::seconds(3));
  Tester::assertAllDead();
}

TEST_F(GCTest, arrayYoungTest) {
  Tester::reset(5);
  gc_ptr<Tester[]> ptr = make_gc<Tester[]>(5);
  Tester::assertAllCreated();
  Tester::assertAllAlive();
  ptr = nullptr;
  GcReference::collect();
  std::this_thread::sleep_for(std::chrono::seconds(3));
  Tester::assertAllDead();
}

