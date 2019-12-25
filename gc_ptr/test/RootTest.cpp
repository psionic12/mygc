//
// Created by liu on 19-12-6.
//

#include <gtest/gtest.h>

#include "../headers/gc_ptr.h"

class RootTest : public testing::Test {};

mygc::gc_ptr<int> global; // root 1

TEST_F(RootTest, rootTest) {
  std::set<mygc::GcReference *> roots;
  {
    mygc::gc_ptr<float> local; // root 2
    static thread_local mygc::gc_ptr<char> threadLocal; // root 3
    auto *heap = new mygc::gc_ptr<long>(); // root 4, this is also a root since it is not allocated in GC heap
    mygc::gc_ptr<mygc::gc_ptr<int>>
        local2 = mygc::make_gc<mygc::gc_ptr<int>>(); //root5 this is also a root, but the inner is not.
    roots = {(mygc::GcReference *) &global,
             (mygc::GcReference *) &local,
             (mygc::GcReference *) &threadLocal,
             (mygc::GcReference *) &(*heap),
             (mygc::GcReference *) &local2};
    for (auto *p : roots) {
      ASSERT_NE(mygc::GcReference::getRoots().find(p), mygc::GcReference::getRoots().end());
    }
    delete heap;
  }
}

