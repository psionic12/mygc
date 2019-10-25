//
// Created by liu on 19-10-23.
//

#ifndef MYGC_CHUNK_H
#define MYGC_CHUNK_H

#include <cstddef>
#include <mutex>
namespace mygc {

class Heap {
 public:
  Heap(size_t size) : mSize(size), mStart(new char[size]), mTail(mStart), mEnd(mStart + size) {}
  virtual ~Heap() {
    delete[] mStart;
  }
  void *allocate(size_t size);
  void clear();
 private:
  size_t mSize;
  char *mStart;
  char *mTail;
  char *mEnd;
  std::mutex mMutex;
};
}//namespace mygc

#endif //MYGC_CHUNK_H
