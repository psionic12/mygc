//
// Created by liu on 19-10-25.
//

#ifndef MYGC_IDESCRIPTOR_H
#define MYGC_IDESCRIPTOR_H
#include <vector>
class IDescriptor {
 public:
  virtual std::pair<const size_t, const std::vector<size_t> &> getIndices() = 0;
  virtual void destructor(void *object) = 0;
};
#endif //MYGC_IDESCRIPTOR_H
