//
// Created by liu on 19-11-1.
//

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <thread>

#include "../src/stop_the_world.h"

void *workerFunc(void *index) {
  while (1) {
    std::cout <<  *(int*) index << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

int main(int argc, char *argv[]) {
  stop_the_world_init();
  std::set<pthread_t> threads;
  for (int i = 0; i < 4; i++) {
    int *index = new int;
    *index = i;
    pthread_t threadId;
    int err = pthread_create(&threadId, nullptr, &workerFunc, index);
    if (err) {
      std::cout << "Thread creation failed : " << strerror(err);
      return err;
    } else {
      std::cout << "Thread Created with ID : " << threadId << std::endl;
    }
    threads.emplace(threadId);
  }

  char c;
  while (true) {
    c = getchar();
    std::cout << "cmd: " << c << std::endl;
    switch (c) {
      case 's': {
        stop_the_world(threads);
        break;
      }
      case 'c':restart_the_world();
        break;
      case '\n':break;
      default: std::cout << "unknown cmd " << "\"" << c << "\"" << std::endl;
    }
  }

}
