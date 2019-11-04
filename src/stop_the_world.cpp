//
// Created by liu on 19-11-1.
//

#include <signal.h>
#include <glog/logging.h>
#include <mutex>
#include <condition_variable>

#include "stop_the_world.h"

#ifndef MYGC_STOP_SIGNAL
#define MYGC_STOP_SIGNAL SIGRTMIN + ('m' + 'y' + 'g' + 'c') % (SIGRTMAX - SIGRTMIN)
#endif

std::mutex sThreadBlocker;
std::condition_variable sBlockerCondition;
std::condition_variable sAcknowledgeCondition;
unsigned long gTotalThreads = 0;
unsigned long sAcknowledgeThreads = 0;

void stop_handler(int signal) {
  LOG(INFO) << "tid: " << pthread_self() << " caught signal: " << signal << std::endl;
  std::unique_lock<std::mutex> lk(sThreadBlocker);
  sAcknowledgeThreads++;
  if (sAcknowledgeThreads == gTotalThreads) {
    sAcknowledgeCondition.notify_all();
    LOG(INFO) << "notify all thread is stopped";
  }
  sBlockerCondition.wait(lk);
  LOG(INFO) << "handler resumed";
}

void stop_the_world(const std::set<pthread_t> &threads) {
  auto self = pthread_self();
  for (auto thread : threads) {
    if (thread != self) {
      gTotalThreads++;
      pthread_kill(thread, MYGC_STOP_SIGNAL);
      LOG(INFO) << "send stop signal to tid: " << thread;
    }
  }
  {
    std::unique_lock<std::mutex> lk(sThreadBlocker);
    if (sAcknowledgeThreads != gTotalThreads) {
      LOG(INFO) << "waiting for threads to stop";
      sAcknowledgeCondition.wait(lk);
    } else {
      LOG(INFO) << "all thread is already stopped";
    }
  }
}
void restart_the_world() {
  sBlockerCondition.notify_all();
  gTotalThreads = 0;
  sAcknowledgeThreads = 0;
}
void stop_the_world_init() {
  // init signal for stopping threads
  //TODO use sigact()
  signal(MYGC_STOP_SIGNAL, stop_handler);
}
