//
// Created by liu on 19-11-12.
//

#ifndef MYGC_OLDGENERATION_H
#define MYGC_OLDGENERATION_H

#include "Block.h"
#include "PendingDestructors.h"
namespace mygc {
class ObjectRecord;
class OldGeneration {
 public:
  ObjectRecord *copyToStopped(ObjectRecord *from);
  ~OldGeneration() {
    for (auto &block : mBlocks) {
      delete block;
    }
  }
  void onCollectionFinished();
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
  PendingDestructors mPendingDestructors;

};
}

#endif //MYGC_OLDGENERATION_H
