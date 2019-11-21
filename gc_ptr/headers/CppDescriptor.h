//
// Created by liu on 19-10-25.
//

#ifndef MYGC_CPPDESCRIPTOR_H
#define MYGC_CPPDESCRIPTOR_H

#include <IDescriptor.h>

template<typename T>
class CppDescriptor : public mygc::IDescriptor {
 public:
  CppDescriptor(std::vector<size_t> &&indices) : mIndices(std::move(indices)) {}
  std::pair<const size_t, const std::vector<size_t> &> getIndices() override {
    return {1, mIndices};
  }
  void destructor(void *object) override {
    static_cast<const T *>(object)->~T();
  }
 private:
  std::vector<size_t> mIndices;
};

template<typename T, size_t SIZE>
class CppDescriptor<T[SIZE]> : public mygc::IDescriptor {
 public:
  CppDescriptor(IDescriptor *descriptor) : mIndices(descriptor->getIndices().second) {}
  std::pair<const size_t, const std::vector<size_t> &> getIndices() override {
    return {SIZE, mIndices};
  }
  void destructor(void *object) override {
    for (size_t i = 0; i < SIZE; i++) {
      (static_cast<const T *>(object) + i)->~T();
    }
  }
  size_t typeSize() override {
    return sizeof(T);
  }
 private:
  const std::vector<size_t> &mIndices;
};

#endif //MYGC_CPPDESCRIPTOR_H
