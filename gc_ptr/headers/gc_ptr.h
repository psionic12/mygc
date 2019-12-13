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
template<typename _Tp, typename = void>
class _TypeRegister {
 public:
  _TypeRegister() {
    registerType();
  }
  static void registerType(std::vector<size_t> &&indices = {}, bool complete = false) {
    size_t typeId = typeid(_Tp).hash_code();
    GcReference::registerType(typeId, sizeof(_Tp), std::move(indices), nullptr, complete);
  }
};
template<typename _Tp>
class _TypeRegister<_Tp, typename std::enable_if_t<!std::is_trivially_destructible<_Tp>::value>> {
 public:
  _TypeRegister() {
    registerType();
  }
  static void registerType(std::vector<size_t> &&indices = {}, bool complete = false) {
    size_t typeId = typeid(_Tp).hash_code();
    GcReference::registerType(typeId, sizeof(_Tp), std::move(indices), &_TypeRegister::destruct, complete);
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
  }
  gc_ptr &
  operator=(nullptr_t) noexcept {
    mGcReference.update(nullptr);
    return *this;
  }
//  ~gc_ptr() {
//    if (!GcReference::isInYoungGeneration(this)) {
//      GcReference::removeRoots(&mGcReference);
//    }
//  }
  _Tp *operator->() {
    return (_Tp *) mGcReference.getReference();
  }
 private:
  template<typename _Up, typename... _Args>
  friend typename _MakeGc<_Up>::__single_object
  make_gc(_Args &&... __args);
  //this is used for make_gc(), which do not add roots, while calculate indices
  explicit gc_ptr(GcReference gcReference) {
    mThreadRegister;
    mGcReference = gcReference;
    createIndices();
  }
  static bool mIndexing; // do not make it atomic, may calculate several time but save time for make_gc later
  GcReference mGcReference;
  static thread_local _ThreadRegister mThreadRegister;
  void createIndices() {
    if (!_AddressBase::empty() && GcReference::isInYoungGeneration(this)) {
      _AddressBase::back().second.push_back(this);
    }
  }
};
template<typename _Tp>
class gc_ptr<_Tp[]> {};
template<typename _Tp>
bool gc_ptr<_Tp>::mIndexing(false);
template<typename _Tp>
thread_local _ThreadRegister gc_ptr<_Tp>::mThreadRegister;

template<typename _Tp>
struct _MakeGc { typedef gc_ptr<_Tp> __single_object; };
template<typename _Tp>
struct _MakeGc<_Tp[]> { typedef gc_ptr<_Tp[]> __array; };
template<typename _Tp, typename... _Args>
typename _MakeGc<_Tp>::__single_object
make_gc(_Args &&... __args) {
  static _TypeRegister<_Tp> mTypeRegister;
  auto typeId = typeid(_Tp).hash_code();
  bool completed = GcReference::isCompletedDescriptor(typeId);
  GcReference reference;
  reference.gcAllocate(typeId, 0);
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
    _TypeRegister<_Tp>::registerType(std::move(offsets), true);
    _AddressBase::pop();
  }
  return gc_ptr<_Tp>(reference);
}
template<typename _Tp>
inline typename _MakeGc<_Tp>::__array
make_unique(size_t __num) {

}
}//namespace mygc

#endif //MYGC_GC_PTR_H
