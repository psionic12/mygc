//
// Created by liu on 19-11-12.
//

#ifndef MYGC_OLDGENERATION_H
#define MYGC_OLDGENERATION_H

#include "Block.h"
namespace mygc {
class ObjectRecord;
class OldGeneration {
 public:
  OldGeneration();
  ObjectRecord *copyTo(ObjectRecord *from);
 private:
  Block<(1 << 6)> mBlock6;
  Block<(1 << 7)> mBlock7;
  Block<(1 << 8)> mBlock8;
  Block<(1 << 9)> mBlock9;
  Block<(1 << 10)> mBlock10;
  Block<(1 << 11)> mBlock11;
  Block<(1 << 12)> mBlock12;

};
}

#endif //MYGC_OLDGENERATION_H
