//
// Created by liu on 19-11-1.
//

#include <signal.h>
#include "Tools.h"
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
//  GCLOG("tid: %u, caught signal: %u", pthread_self(), signal);
  std::unique_lock<std::mutex> lk(sThreadBlocker);
  sAcknowledgeThreads++;
  if (sAcknowledgeThreads == gTotalThreads) {
    sAcknowledgeCondition.notify_all();
//    GCLOG("notify all thread is stopped");
  }
  sBlockerCondition.wait(lk);
//  GCLOG("handler resumed");
}

void stop_the_world(const std::set<pthread_t> &threads) {
  auto self = pthread_self();
  for (auto thread : threads) {
    if (thread != self) {
      std::unique_lock<std::mutex> lk(sThreadBlocker);
      gTotalThreads++;
      lk.unlock();
      if (pthread_kill(thread, MYGC_STOP_SIGNAL) < 0) {
        perror("sigaction");
        exit(-1);
      } else {
//        GCLOG("send stop signal to tid: %u", thread);
      }
    }
  }
  {
    std::unique_lock<std::mutex> lk(sThreadBlocker);
    if (sAcknowledgeThreads != gTotalThreads) {
//      GCLOG("waiting for threads to stop");
      sAcknowledgeCondition.wait(lk);
    } else {
//      GCLOG("all thread is already stopped");
    }
//    GCLOG("stop successfully");
  }
}
void restart_the_world() {
  sBlockerCondition.notify_all();
  gTotalThreads = 0;
  sAcknowledgeThreads = 0;
}
void stop_the_world_init() {
  // init signal for stopping threads
  struct sigaction act;
  memset(&act, '\0', sizeof(act));
  act.sa_handler = stop_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_RESTART;
  sigaction (MYGC_STOP_SIGNAL, &act, nullptr);
}
