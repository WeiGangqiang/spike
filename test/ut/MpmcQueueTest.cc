#include "gtest/gtest.h"
#include "MpmcQueue.h"
#include "TccCalcTask.h"

using namespace tcc;
class MpmcQueueTest : public testing::Test {


};

using TccCalcQueue = MpmcQueue<TccCalcTask *>;

TEST(MpmcQueueTest, highway_and_normal_compare){

  TccCalcQueue queue(10);
  TccCalcTask task;

  queue.Push(&task);
  ASSERT_EQ(queue.GetSize(), 1);

  TccCalcTask* recv;
  queue.Pop(recv);

  ASSERT_EQ(queue.GetSize(), 0);
  ASSERT_EQ(recv, &task);
}


