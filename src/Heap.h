//
// Created by liu on 19-10-23.
//

#ifndef MYGC_CHUNK_H
#define MYGC_CHUNK_H

#include <cstddef>
namespace mygc {

class Heap {
 public:
  Heap(size_t size);
  virtual ~Heap();
  void *allocateLocked(size_t size);
  void clearLocked();
  bool inHeapLocked(void *ptr);
 private:
  size_t mSize;
  char *mStart;
  char *mTail;
  char *mEnd;
};
}//namespace mygc

#endif //MYGC_CHUNK_H
