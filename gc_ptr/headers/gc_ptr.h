//
// Created by liu on 19-10-25.
//

#ifndef MYGC_GC_PTR_H
#define MYGC_GC_PTR_H

#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <memory>
#include <glog/logging.h>
#include "GcReference.h"
namespace mygc {
class ThreadRegister {
 public:
  ThreadRegister() {
    GcReference::attachThread(pthread_self());
  }
  ~ThreadRegister() {
    GcReference::detachThread(pthread_self());
  }
};
class AddressBase {
 public:
  static void push(void *base) {
    getBases().push_back({base, {}});
  }
  static std::pair<void *, std::vector<void *>> &back() {
    return getBases().back();
  }
  static void pop() {
    getBases().pop_back();
  }
  static bool empty() {
    return getBases().empty();
  }
 private:
  static std::vector<std::pair<void *, std::vector<void *>>> &getBases() {
    static thread_local std::vector<std::pair<void *, std::vector<void *>>> v;
    return v;
  }
};
template<typename T>
class TypeRegister {
 public:
  TypeRegister() {
    size_t typeId = typeid(T).hash_code();
    GcReference::registeredType(typeId, sizeof(T), {}, &TypeRegister::destruct, false);
  }
  static void destruct(void *t) {
    ((T *) t)->~T();
  }
};

template<typename T>
class gc_ptr {
 public:
  gc_ptr() {
    mThreadRegister;
    if (!GcReference::isInYoungGeneration(this)) {
      GcReference::addRoots(&mGcReference);
    }
    createIndices();
  }
  ~gc_ptr() {
    if (!GcReference::isInYoungGeneration(this)) {
      GcReference::removeRoots(&mGcReference);
    }
  }
  explicit gc_ptr(GcReference gcReference) : gc_ptr() {
    mGcReference = gcReference;
  }
 private:
  GcReference mGcReference;
  static thread_local ThreadRegister mThreadRegister;
  static TypeRegister<T> mTypeRegister;
  void createIndices() {
    mTypeRegister;
    if (!AddressBase::empty() && GcReference::isInYoungGeneration(this)) {
      AddressBase::back().second.push_back(this);
    }
  }
};
template<typename T>
TypeRegister<T> gc_ptr<T>::mTypeRegister;
template<typename T>
thread_local ThreadRegister gc_ptr<T>::mThreadRegister;

template<typename T>
struct _MakeGc { typedef gc_ptr<T> __single_object; };
template<typename T, typename... _Args>
inline typename _MakeGc<T>::__single_object
make_gc(_Args &&... __args) {
  auto typeId = typeid(T).hash_code();
  bool completed = GcReference::isCompletedDescriptor(typeId);
  GcReference reference;
  reference.gcAlloca(typeId);
  void *ptr = reference.getReference();
  if (!completed) AddressBase::push(ptr);
  auto *t = new(ptr) T(std::forward<_Args>(__args)...);
  if (!completed) {
    auto &pair = AddressBase::back();
    void *base = pair.first;
    std::vector<size_t> offsets;
    for (auto *child : pair.second) {
      offsets.push_back((char *) child - (char *) base);
    }
    GcReference::registeredType(typeId,
                                sizeof(T),
                                {1, std::move(offsets)},
                                &TypeRegister<T>::destruct,
                                true);
    AddressBase::pop();
  }
  return gc_ptr<T>(reference);
}

//template<typename T, typename... Args, size_t SIZE>
//class gc_ptr<T[SIZE], Args...> : GcReference {
// public:
//  gc_ptr(Args &&... args) {
//    auto ptr = new(getReference()) T[SIZE]{std::forward<Args>(args)...};
//  }
// private:
//  static CppClassRegister<T> mRegister;
//};

}//namespace mygc

#endif //MYGC_GC_PTR_H
