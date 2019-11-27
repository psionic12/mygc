//
// Created by liu on 19-11-12.
//

#ifndef MYGC_OLDGENERATION_H
#define MYGC_OLDGENERATION_H
#include <mutex>

#include "Block.h"
#include "ObjectRecord.h"
namespace mygc {
class OldGeneration {
 public:
  OldGeneration();
  OldRecord *copyToStopped(mygc::YoungRecord *from);
  ~OldGeneration() {
    for (auto &block : mBlocks) {
      delete block;
    }
  }
  void onScanBegin();
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
  OldRecord *mLivingFinalizer;
  OldRecord *mDeadFinalizer;
  std::mutex mDeadFinalizerMutex;
};
}

#endif //MYGC_OLDGENERATION_H
