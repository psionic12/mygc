//
// Created by liu on 19-10-30.
//

#ifndef MYGC_REFERENCE_H
#define MYGC_REFERENCE_H

#include <cstddef>
#include <utility>
#include <vector>
namespace mygc {
class Record;
class GcReference {
 private:
  GcReference();
  Record *mPtr = nullptr;
 public:
  explicit GcReference(size_t typeId);
  GcReference(size_t typeId, size_t typeSize);
  virtual ~GcReference();
  void *getReference();
  Record *getRecord();
  void update(Record *newRecord);
  bool isRoot();
  static void registeredType(size_t typeId,
                             size_t typeSize,
                             std::pair<const size_t, const std::vector<size_t>> &&indices,
                             void (*destructor)(void *object) = nullptr);
  static bool isRegistered(size_t typeId);
};

}//namespace mygc

#endif //MYGC_REFERENCE_H
