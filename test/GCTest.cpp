//
// Created by liu on 19-10-31.
//

#include <gtest/gtest.h>
#include "GcReference.h"
using namespace mygc;
class GcTest : public testing::Test {};

class Tester {
 public:
  ~Tester() {

  }
};

TEST_F(GcTest, test) {
  GcReference::registeredType(1, 1024, {}, nullptr, true);
  GcReference gcReference;
  GcReference::addRoots(&gcReference);
  for (int i = 0; i < 10; i++) {
    gcReference.gcAlloca(1);
  }
}