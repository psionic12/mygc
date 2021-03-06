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
#include "RecordList.h"
namespace mygc {
class OldGeneration {
 public:
  OldGeneration();
  OldRecord *copyFromYoungSTW(mygc::YoungRecord *from);
  ~OldGeneration();
  void onScanEnd();
  /// return true if marked, return false if record has already marked
  bool mark(OldRecord *record);
  void pickNonTrivial(OldRecord *record);
  size_t getAllocatedSize();
  static inline constexpr size_t getMaxBlockSize() {
    return 1 << 12;
  }
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
  OldNonTrivialList mWhiteList;// objects with finalizer in this list are alive
  OldNonTrivialList mGrayList;// objects with finalizer in this list are alive
  OldNonTrivialList mBlackList;// objects with finalizer in this list are alive
  bool mTerminate;
  std::mutex mBlackFinalizerMutex;
  std::thread mScavenger;
  std::condition_variable mCV;
  void scavenge();
};
}

#endif //MYGC_OLDGENERATION_H
