//
// Created by liu on 19-12-4.
//

#ifndef MYGC_YOUNGGENERATIONS_H
#define MYGC_YOUNGGENERATIONS_H

#include <unordered_map>
#include <thread>

#include "YoungGeneration.h"
#include "YoungGenerationPool.h"
namespace mygc {
class YoungGenerations {
 public:
  YoungGeneration *getMine();
  void onScanEnd();
 private:
  std::unordered_map<std::thread::id, std::unique_ptr<YoungGeneration>> mAttachedYoungGenerations;
  YoungGenerationPool mYoungPool;
};
}//namespace mygc
#endif //MYGC_YOUNGGENERATIONS_H
