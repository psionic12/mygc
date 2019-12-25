//
// Created by liu on 19-11-14.
//
#include <gtest/gtest.h>

class AlignmentTest : public testing::Test {};

TEST_F(AlignmentTest, alignmentTest) {
  constexpr int base = sizeof(void *);

  char *charPtr = new char;
  short *shortPtr = new short;
  int *intPtr = new int;
  float *floatPtr = new float;
  double *doublePtr = new double;
  long *longPtr = new long;
  char *arrayPtr = new char[111];

  ASSERT_EQ(reinterpret_cast<size_t>(charPtr) % base, 0);
  ASSERT_EQ(reinterpret_cast<size_t>(shortPtr) % base, 0);
  ASSERT_EQ(reinterpret_cast<size_t>(floatPtr) % base, 0);
  ASSERT_EQ(reinterpret_cast<size_t>(doublePtr) % base, 0);
  ASSERT_EQ(reinterpret_cast<size_t>(longPtr) % base, 0);
  ASSERT_EQ(reinterpret_cast<size_t>(arrayPtr) % base, 0);

  delete charPtr;
  delete shortPtr;
  delete intPtr;
  delete floatPtr;
  delete doublePtr;
  delete longPtr;
  delete[] arrayPtr;
}