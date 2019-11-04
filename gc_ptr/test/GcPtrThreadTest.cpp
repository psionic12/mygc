//
// Created by liu on 19-11-4.
//
#include <set>
#include <thread>
#include <mutex>
#include <gtest/gtest.h>

#include "gc_ptr.h"
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
  mygc::gc_ptr<int> p1;
  mygc::gc_ptr<int> p2;
  mygc::gc_ptr<int> p3;
  while (true) {
    sleep(1);
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
  sleep(2);
  {
    std::lock_guard<std::mutex> guard(gMutex);
    ASSERT_EQ(gThreads, mygc::GarbageCollector::getCollector().getAttachedThreads());
    ASSERT_EQ(gThreadCounts, mygc::GarbageCollector::getCollector().getAttachedThreads().size());
    gStop = true;
  }
}