//
// Created by Liu Yafei on 10/27/19.
//
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <gc_ptr.h>

auto gUuid = boost::uuids::random_generator()();

mygc::PtrHeaders::PtrHeaders() : mPtr(nullptr) {
  std::copy(gUuid.data, gUuid.data + gUuid.size(), mUuid);
}
bool mygc::PtrHeaders::check() {
  return !memcmp(mUuid, gUuid.data, gUuid.size());
}
