//
// Created by liu on 19-11-26.
//

#ifndef MYGC_YOUNGGENERATIONPOOL_H
#define MYGC_YOUNGGENERATIONPOOL_H
#include <mutex>
#include <vector>
#include <memory>
#include <thread>
#include <condition_variable>
#include "YoungGeneration.h"
namespace mygc {
class YoungGenerationPool {
 public:
  YoungGenerationPool();
  ~YoungGenerationPool();
  void putDirtyGeneration(std::unique_ptr<YoungGeneration> &&generation);
  std::unique_ptr<YoungGeneration> getCleanGeneration();
 private:
  std::mutex mMutex;
  bool mTerminate;
  std::condition_variable mCV;
  std::vector<std::unique_ptr<YoungGeneration>> mClean;
  std::vector<std::unique_ptr<YoungGeneration>> mDirty;
  std::thread mScavenger;
  void scavenge();
};
}//namespace mygc

#endif //MYGC_YOUNGGENERATIONPOOL_H
