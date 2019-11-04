//
// Created by liu on 19-11-1.
//

#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <gtest/gtest.h>
#include <set>
#include "../src/stop_the_world.h"

class StopTheWorldTest : public testing::Test {};

int workers = 4;
std::vector<int> v(workers);

std::set<pthread_t> threads;

void *workerFunction(void *index) {
  while (true) {
    int i = *(int*) index;
    v[i]++;
    sleep(0.5);
  }
}

TEST_F(StopTheWorldTest, stop_the_world_test) {
  for (int i = 0; i < workers; i++) {
    pthread_t threadId;
    int *index = new int;
    *index = i;
    int err = pthread_create(&threadId, nullptr, &workerFunction, index);
    ASSERT_EQ(err, 0);
    threads.emplace(threadId);
  }
  sleep(3);
  stop_the_world_init();
  stop_the_world(threads);
  std::vector<int> v1(v);
  sleep(3);
  ASSERT_EQ(v, v1);
  restart_the_world();
  sleep(3);
  for(auto id : threads) {
    pthread_cancel(id);
  }
  ASSERT_NE(v, v1);
}


