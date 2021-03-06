//
// Created by liu on 19-10-30.
//

#ifndef MYGC_REFERENCE_H
#define MYGC_REFERENCE_H

#include <cstddef>
#include <utility>
#include <vector>
#include <pthread.h>
#include <set>
namespace mygc {
class Record;
class GcReference {
 private:
  Record *mPtr = nullptr;
 public:
  GcReference() = default;
  GcReference(Record *ptr);
  void gcAllocate(size_t typeId, size_t counts = 1);
  void *getReference();
  Record *getRecord();
  void update(Record *newRecord);
  static bool isInYoungGeneration(void *ptr);
  static void registerType(size_t typeId,
                           size_t typeSize,
                           std::vector<size_t> &&indices,
                           void (*destructor)(void *object),
                           bool completed);
  static bool isCompletedDescriptor(size_t typeId);
  static void addRoots(GcReference *ptr);
  static void removeRoots(GcReference *ptr);
  static void attachThread(pthread_t thread);
  static void detachThread(pthread_t thread);
  explicit operator bool() const noexcept {
    return mPtr != nullptr;
  }

  // used for test
  static std::vector<size_t> getIndices(size_t typeId);
  static std::set<pthread_t> getAttachedThreads();
  static std::set<GcReference *> getRoots();
  static void collect();// do not use this to manually collect objects,
  // this only collect the generation which belongs to the caller thread
};

}//namespace mygc

#endif //MYGC_REFERENCE_H
