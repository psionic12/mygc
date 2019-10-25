//
// Created by liu on 19-10-25.
//

#ifndef MYGC_CPPDESCRIPTOR_H
#define MYGC_CPPDESCRIPTOR_H

#include <IDescriptor.h>

template <typename T>
class CppDescriptor : public IDescriptor {
 public:
  CppDescriptor( std::vector<size_t> &&indices) : mIndices(std::move(indices)) {}
  const std::vector<size_t> &getIndices() override {
    return mIndices;
  }
  void destructor(void *object) override {
    static_cast<const T*>(object)->~T();
  }
 private:
  std::vector<size_t> mIndices;
};

#endif //MYGC_CPPDESCRIPTOR_H
