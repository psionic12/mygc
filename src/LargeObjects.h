//
// Created by liu on 19-12-2.
//

#ifndef MYGC_LARGEOBJECTS_H
#define MYGC_LARGEOBJECTS_H

#include <mutex>
#include <condition_variable>
#include <thread>
#include "ObjectRecord.h"
#include "RecordList.h"
namespace mygc {

class LargeObjects {
 public:
  LargeObjects();
  LargeRecord *allocate(TypeDescriptor &descriptor);
  void onScanEnd();
  void mark(LargeRecord *record);
 private:
  LargeObjectList mWhiteList;
  LargeObjectList mGrayList;
  LargeObjectList mBlackList;
  std::mutex mBlackListMutex;
  std::condition_variable mCV;
  std::thread mScavenger;
  void scavenge();
};

}//namespace mygc

#endif //MYGC_LARGEOBJECTS_H
