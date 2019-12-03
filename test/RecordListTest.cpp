//
// Created by liu on 19-12-3.
//

#include <gtest/gtest.h>
#include "../src/RecordList.h"

class RecordListTest : public testing::Test {};

TEST_F(RecordListTest, addTest) {
  mygc::NonTrivialList list;
  ASSERT_EQ(list.getHead(), nullptr);
  mygc::Record record1;
  ASSERT_EQ(record1.nonTrivialNode.pre, nullptr);
  ASSERT_EQ(record1.nonTrivialNode.pre, nullptr);
  list.add(&record1);
  ASSERT_EQ(list.getHead(), &record1);
  mygc::Record record2;
  list.add(&record2);
  ASSERT_EQ(list.getHead(), &record2);
  ASSERT_EQ(record1.nonTrivialNode.pre, &record2);
  ASSERT_EQ(record2.nonTrivialNode.next, &record1);
  ASSERT_EQ(record1.nonTrivialNode.next, nullptr);
  ASSERT_EQ(record2.nonTrivialNode.pre, nullptr);
}

TEST_F(RecordListTest, removeMiddleTest) {
  mygc::NonTrivialList list;
  mygc::Record record1;
  mygc::Record record2;
  mygc::Record record3;
  list.add(&record3);
  list.add(&record2);
  list.add(&record1);

  list.remove(&record2);
  ASSERT_EQ(list.getHead(), &record1);
  ASSERT_EQ(record2.nonTrivialNode.pre, nullptr);
  ASSERT_EQ(record2.nonTrivialNode.next, nullptr);
  ASSERT_EQ(record1.nonTrivialNode.pre, nullptr);
  ASSERT_EQ(record1.nonTrivialNode.next, &record3);
  ASSERT_EQ(record3.nonTrivialNode.pre, &record1);
  ASSERT_EQ(record3.nonTrivialNode.next, nullptr);
}

TEST_F(RecordListTest, removeTailTest) {
  mygc::NonTrivialList list;
  mygc::Record record1;
  mygc::Record record2;
  mygc::Record record3;
  list.add(&record3);
  list.add(&record2);
  list.add(&record1);

  list.remove(&record3);
  ASSERT_EQ(list.getHead(), &record1);
  ASSERT_EQ(record3.nonTrivialNode.pre, nullptr);
  ASSERT_EQ(record3.nonTrivialNode.next, nullptr);
  ASSERT_EQ(record1.nonTrivialNode.pre, nullptr);
  ASSERT_EQ(record1.nonTrivialNode.next, &record2);
  ASSERT_EQ(record2.nonTrivialNode.pre, &record1);
  ASSERT_EQ(record2.nonTrivialNode.next, nullptr);
}

TEST_F(RecordListTest, removeHeadTest) {
  mygc::NonTrivialList list;
  mygc::Record record1;
  mygc::Record record2;
  mygc::Record record3;
  list.add(&record3);
  list.add(&record2);
  list.add(&record1);

  list.remove(&record1);
  ASSERT_EQ(list.getHead(), &record2);
  ASSERT_EQ(record1.nonTrivialNode.pre, nullptr);
  ASSERT_EQ(record1.nonTrivialNode.next, nullptr);
  ASSERT_EQ(record2.nonTrivialNode.pre, nullptr);
  ASSERT_EQ(record2.nonTrivialNode.next, &record3);
  ASSERT_EQ(record3.nonTrivialNode.pre, &record2);
  ASSERT_EQ(record3.nonTrivialNode.next, nullptr);
}