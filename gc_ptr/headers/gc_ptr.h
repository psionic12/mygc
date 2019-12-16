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
struct _MakeGc<_Tp[]>;

template<typename _Tp>
class __gc_ptr_impl {
 public:
  __gc_ptr_impl() {
    mThreadRegister;
    if (!GcReference::isInYoungGeneration(this)) {
      GcReference::addRoots(&mGcReference);
    }
  }
  __gc_ptr_impl(const __gc_ptr_impl<_Tp> &ptr) : __gc_ptr_impl() {
    mGcReference = ptr.mGcReference;
  }
  __gc_ptr_impl(__gc_ptr_impl<_Tp> &&ptr) : __gc_ptr_impl() {
    mGcReference = ptr.mGcReference;
    ptr.mGcReference.update(nullptr);
  }
  explicit __gc_ptr_impl(GcReference gcReference) : __gc_ptr_impl() {
    mGcReference = gcReference;
  }
  ~__gc_ptr_impl() {
    if (!GcReference::isInYoungGeneration(this)) {
      GcReference::removeRoots(&mGcReference);
    }
  }
  __gc_ptr_impl &operator=(const __gc_ptr_impl &ptr) = default;
  __gc_ptr_impl &operator=(__gc_ptr_impl &&ptr) noexcept = default;
  GcReference &getReference() {
    return mGcReference;
  }

 private:
  static thread_local _ThreadRegister mThreadRegister;
  GcReference mGcReference;
};
template<typename _Tp>
thread_local _ThreadRegister __gc_ptr_impl<_Tp>::mThreadRegister;
template<typename _Tp>
class gc_ptr {
 public:
  gc_ptr() : _M_t() {
    createIndices();
  }
  explicit gc_ptr(GcReference gcReference) : _M_t(gcReference) {}
  gc_ptr(const gc_ptr &ptr) : _M_t(ptr._M_t) {
    createIndices();
  }
  gc_ptr(gc_ptr &&ptr) : _M_t(ptr._M_t) {
    createIndices();
  }
  gc_ptr &operator=(nullptr_t) noexcept {
    _M_t.getReference().update(nullptr);
    return *this;
  }
  gc_ptr &operator=(const gc_ptr &ptr) {
    _M_t = ptr._M_t;
    return *this;
  }
  gc_ptr &operator=(gc_ptr &&ptr) noexcept {
    _M_t = ptr._M_t;
    ptr._M_t.getReference().update(nullptr);
    return *this;
  }
  _Tp *operator->() {
    return (_Tp *) _M_t.getReference().getReference();
  }
 private:
  template<typename _Up, typename... _Args>
  friend typename _MakeGc<_Up>::__single_object
  make_gc(_Args &&... __args);
  template<typename _Up>
  friend typename _MakeGc<_Up>::__array
  make_gc(size_t __num);
  static bool mIndexing; // do not make it atomic, may calculate several time but save time for make_gc later
  void createIndices() {
    if (!_AddressBase::empty() && GcReference::isInYoungGeneration(this)) {
      _AddressBase::back().second.push_back(this);
    }
  }
  __gc_ptr_impl<_Tp> _M_t;
};
template<typename _Tp>
bool gc_ptr<_Tp>::mIndexing(false);

template<typename _Tp>
class gc_ptr<_Tp[]> {
 public:
  gc_ptr() : _M_t() {}
  gc_ptr &operator=(nullptr_t) noexcept {
    _M_t.getReference().update(nullptr);
    return *this;
  }
 private:
  template<typename _Up>
  friend typename _MakeGc<_Up>::__array
  make_gc(size_t __num);
  explicit gc_ptr(GcReference gcReference) : _M_t(gcReference) {}
  __gc_ptr_impl<_Tp> _M_t;
};

template<typename _Tp>
struct _MakeGc { typedef gc_ptr<_Tp> __single_object; };
template<typename _Tp>
struct _MakeGc<_Tp[]> { typedef gc_ptr<_Tp[]> __array; };
template<typename _Tp, size_t _Bound>
struct _MakeGc<_Tp[_Bound]> { struct __invalid_type {}; };
template<typename _Tp, typename... _Args>
typename _MakeGc<_Tp>::__single_object
make_gc(_Args &&... __args) {
//  static thread_local _ThreadRegister threadRegister;
  static _TypeRegister<_Tp> mTypeRegister;
  auto typeId = typeid(_Tp).hash_code();
  bool completed = GcReference::isCompletedDescriptor(typeId);
  GcReference reference;
  reference.gcAllocate(typeId);
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
typename _MakeGc<_Tp>::__array
make_gc(size_t __num) {
  typedef std::remove_extent_t<_Tp> ElementType;
  static _TypeRegister<ElementType> mTypeRegister;
  auto typeId = typeid(ElementType).hash_code();
  bool completed = GcReference::isCompletedDescriptor(typeId);
  GcReference reference;
  reference.gcAllocate(typeId, __num);
  void *ptr = reference.getReference();
  bool shouldIndex = !completed && !gc_ptr<ElementType>::mIndexing;
  if (shouldIndex) {
    gc_ptr<ElementType>::mIndexing = true;
    _AddressBase::push(ptr);
  }
  new(ptr) ElementType(); // construct only one to get the indices
  if (shouldIndex) {
    auto &pair = _AddressBase::back();
    auto &v = _AddressBase::getBases();
    void *base = pair.first;
    std::vector<size_t> offsets;
    for (auto *child : pair.second) {
      offsets.push_back((char *) child - (char *) base);
    }
    _TypeRegister<ElementType>::registerType(std::move(offsets), true);
    _AddressBase::pop();
  }
  new((ElementType *) ptr + 1) ElementType[__num - 1];
  return gc_ptr<_Tp>(reference);
}
template<typename _Tp, typename... _Args>
inline typename _MakeGc<_Tp>::__invalid_type
make_gc(_Args &&...) = delete;
}//namespace mygc

#endif //MYGC_GC_PTR_H
