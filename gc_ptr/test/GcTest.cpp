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

template<int TestId, int Size>
class Tester {
 public:
  Tester() {
    mConstructorCalled = true;
    std::unique_lock<std::mutex> lock(sMutex);
    static int i = 0;
    mId = i++;
    Tester::aliveness[mId] += 1;
    Tester::createdIndex[mId] = mId;
    lock.unlock();

//    DLOG(INFO) << "Tester: " << mId << "(" << this << ")" << std::endl;
  }
  ~Tester() {
    std::unique_lock<std::mutex> lock(sMutex);
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
    std::unique_lock<std::mutex> lock(sMutex);
    for (int i = 0; i < createdIndex.size(); i++) {
      ASSERT_EQ(createdIndex[i], i) << " : v2[" << i << "] is " << createdIndex[i] << std::endl;
    }
  }
  static void assertAllAlive() {
    std::unique_lock<std::mutex> lock(sMutex);
    for (int i = 0; i < aliveness.size(); i++) {
      ASSERT_TRUE(aliveness[i] == 1) << " : v1[" << i << "] is " << aliveness[i] << std::endl;
    }
  }
  static void assertAllDead() {
    std::unique_lock<std::mutex> lock(sMutex);
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
  gc_ptr<Tester<TestId, Size>> mChild = nullptr;
  bool mConstructorCalled = false;
  char mPlaceHolder[Size];

  static std::mutex sMutex;
  static std::vector<int> aliveness;
  static std::vector<int> createdIndex;

};
template<int TestId, int Size>
std::mutex Tester<TestId, Size>::sMutex;
template<int TestId, int Size>
std::vector<int> Tester<TestId, Size>::aliveness{};
template<int TestId, int Size>
std::vector<int> Tester<TestId, Size>::createdIndex{};

TEST_F(GCTest, singleTest) {
  typedef Tester<2, 128> Tester;
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
  typedef Tester<3, 512> Tester;
  Tester::reset(70);
  std::thread thread2([]() {
    gc_ptr<Tester> child;
    gc_ptr<Tester> current;
    for (int i = 0; i < 10; i++) {
      current = make_gc<Tester>();
      current->mChild = child;
      child = current;
      current = nullptr;
    }
    for (int i = 0; i < 10; i++) {
      current = make_gc<Tester>();
    }
    for (int i = 0; i < 10; i++) {
      current = make_gc<Tester>();
    }
    current = nullptr;
    child = nullptr;
    GcReference::collect();
  });
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
  typedef Tester<4, 512> Tester;
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
  typedef Tester<5, 512> Tester;
  Tester::reset(5);
  gc_ptr<Tester[]> ptr = make_gc<Tester[]>(5);
  Tester::assertAllCreated();
  Tester::assertAllAlive();
  ptr = nullptr;
  GcReference::collect();
  std::this_thread::sleep_for(std::chrono::seconds(3));
  Tester::assertAllDead();
}

TEST_F(GCTest, gcLargeTest) {
  typedef Tester<6, 2 << 13> Tester;
  Tester::reset(10);
  std::thread thread2([]() {
    gc_ptr<Tester> t;
    for (int i = 0; i < 10; i++) {
      t = make_gc<Tester>();
      t->mChild = t;
    }
    t = nullptr;
    GcReference::collect();
  });
  thread2.detach();
  gc_ptr<Tester> t;
  for (int i = 0; i < 10; i++) {
    t = make_gc<Tester>();
    ASSERT_EQ(t->constructorCalled(), true);
  }
  t = nullptr;
  GcReference::collect();
  std::this_thread::sleep_for(std::chrono::seconds(3));
  Tester::assertAllCreated();
  Tester::assertAllDead();
}