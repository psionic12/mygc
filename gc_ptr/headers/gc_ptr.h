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
#include <type_traits>
#include <atomic>
#include "GcReference.h"
namespace mygc {
class _ThreadRegister {
 public:
  _ThreadRegister() {
    GcReference::attachThread(pthread_self());
  }
  ~_ThreadRegister() {
    GcReference::detachThread(pthread_self());
  }
};
class _AddressBase {
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
 public:
  static std::vector<std::pair<void *, std::vector<void *>>> &getBases() {
    static thread_local std::vector<std::pair<void *, std::vector<void *>>> v;
    return v;
  }
};
template<typename _Tp>
class _TypeRegister {
 public:
  _TypeRegister() {
    size_t typeId = typeid(_Tp).hash_code();
    GcReference::registeredType(typeId, sizeof(_Tp), {}, &_TypeRegister::destruct, false);
  }
  static void destruct(void *t) {
    ((_Tp *) t)->~_Tp();
  }
};
template<typename _Tp>
struct _MakeGc;
template<typename _Tp>
class gc_ptr {
 public:
  gc_ptr() {
    mThreadRegister;
    if (!GcReference::isInYoungGeneration(this)) {
      GcReference::addRoots(&mGcReference);
    }
    createIndices();
  }
  gc_ptr &
  operator=(nullptr_t) noexcept {
    mGcReference.update(nullptr);
    return *this;
  }
  ~gc_ptr() {
    if (!GcReference::isInYoungGeneration(this)) {
      GcReference::removeRoots(&mGcReference);
    }
  }
  explicit gc_ptr(GcReference gcReference) : gc_ptr() {
    mGcReference = gcReference;
  }
  _Tp *operator->() {
    return (_Tp *) mGcReference.getReference();
  }
 private:
  template<typename _Up, typename... _Args>
  friend typename _MakeGc<_Up>::__single_object
  make_gc(_Args &&... __args);
  static bool mIndexing; // do not make it atomic, may calculate several time but save time for make_gc later
  GcReference mGcReference;
  static thread_local _ThreadRegister mThreadRegister;
  static _TypeRegister<_Tp> mTypeRegister;
  void createIndices() {
    mTypeRegister;
    if (!_AddressBase::empty() && GcReference::isInYoungGeneration(this)) {
      _AddressBase::back().second.push_back(this);
    }
  }
};
template<typename _Tp>
bool gc_ptr<_Tp>::mIndexing(false);
template<typename _Tp>
_TypeRegister<_Tp> gc_ptr<_Tp>::mTypeRegister;
template<typename _Tp>
thread_local _ThreadRegister gc_ptr<_Tp>::mThreadRegister;

template<typename _Tp>
struct _MakeGc { typedef gc_ptr<_Tp> __single_object; };
template<typename _Tp>
struct _MakeGc<_Tp[]> { typedef gc_ptr<_Tp[]> __array; };
template<typename _Tp, typename... _Args>
typename _MakeGc<_Tp>::__single_object
make_gc(_Args &&... __args) {
  auto typeId = typeid(_Tp).hash_code();
  bool completed = GcReference::isCompletedDescriptor(typeId);
  GcReference reference;
  reference.gcAlloca(typeId);
  void *ptr = reference.getReference();
  bool shouldIndex = !completed && !gc_ptr<_Tp>::mIndexing;
  if (shouldIndex) {
    gc_ptr<_Tp>::mIndexing = true;
    _AddressBase::push(ptr);
  }
  new(ptr) _Tp(std::forward<_Args>(__args)...);
  if (shouldIndex) {
    auto &pair = _AddressBase::back();
    auto &v = _AddressBase::getBases();
    void *base = pair.first;
    std::vector<size_t> offsets;
    for (auto *child : pair.second) {
      offsets.push_back((char *) child - (char *) base);
    }
    GcReference::registeredType(typeId,
                                sizeof(_Tp),
                                std::move(offsets),
                                &_TypeRegister<_Tp>::destruct,
                                true);
    _AddressBase::pop();
  }
  return gc_ptr<_Tp>(reference);
}
}//namespace mygc

#endif //MYGC_GC_PTR_H
