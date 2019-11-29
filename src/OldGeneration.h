//
// Created by liu on 19-11-12.
//

#ifndef MYGC_OLDGENERATION_H
#define MYGC_OLDGENERATION_H
#include <mutex>
#include <thread>
#include <condition_variable>

#include "Block.h"
#include "ObjectRecord.h"
#include "FinalizerList.h"
namespace mygc {
class OldGeneration {
 public:
  OldGeneration();
  OldRecord *copyFromYoungSTW(mygc::YoungRecord *from);
  ~OldGeneration() {
    for (auto &block : mBlocks) {
      delete block;
    }
  }
  void onScanEnd();
  void mark(OldRecord *record);
 private:
  IBlock *mBlocks[13]{
      nullptr,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
      new Block<(1 << 6)>,
      new Block<(1 << 7)>,
      new Block<(1 << 8)>,
      new Block<(1 << 9)>,
      new Block<(1 << 10)>,
      new Block<(1 << 11)>,
      new Block<(1 << 12)>
  };
  FinalizerList mWhiteList;// objects with finalizer in this list are alive
  FinalizerList mGrayList;// objects with finalizer in this list are alive
  FinalizerList mBlackList;// objects with finalizer in this list are alive
  std::mutex mBlackFinalizerMutex;
  std::thread mScavenger;
  std::condition_variable mCV;
  void scavenge();
};
}

#endif //MYGC_OLDGENERATION_H
