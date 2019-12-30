//
// Created by liu on 19-11-4.
//
#include <set>
#include <thread>
#include <mutex>
#include <gtest/gtest.h>

#include "../headers/gc_ptr.h"
class GcPtrThreadTest : public testing::Test {};

std::mutex gMutex;
std::set<pthread_t> gThreads{};
int gThreadCounts = 4;
bool gStop = false;

void worker() {
  {
    std::unique_lock<std::mutex> lk(gMutex);
    gThreads.emplace(pthread_self());
  }
  mygc::gc_root<int> p1;
  mygc::gc_root<int> p2;
  mygc::gc_root<int> p3;
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
      std::unique_lock<std::mutex> lk(gMutex);
      if (gStop) break;
    }
  }
}

TEST_F(GcPtrThreadTest, threadTest) {
  for (int i = 0; i < gThreadCounts; i++) {
    std::thread t(worker);
    t.detach();
  }
  std::this_thread::sleep_for(std::chrono::seconds(2));
  {
    std::lock_guard<std::mutex> guard(gMutex);
    auto attached = mygc::GcReference::getAttachedThreads();
    for (auto& element: gThreads) {
      ASSERT_NE(attached.find(element), attached.end());
    }
    gStop = true;
  }

  std::this_thread::sleep_for(std::chrono::seconds(2));
  {
    std::lock_guard<std::mutex> guard(gMutex);
    auto attached = mygc::GcReference::getAttachedThreads();
    for (auto& element: gThreads) {
      ASSERT_EQ(attached.find(element), attached.end());
    }

  }
}