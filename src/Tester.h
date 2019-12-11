//
// Created by liu on 19-12-10.
//

#ifndef MYGC_TESTER_H
#define MYGC_TESTER_H
#include <mutex>
namespace mygc {
struct Tester {
  bool mConstructorCalled;
  std::mutex mMutex;
  char mPlaceHolder[1024];
  int mId;
};
}
#endif //MYGC_TESTER_H
