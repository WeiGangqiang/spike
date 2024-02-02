#include "MemBuf.h"
#include "gtest/gtest.h"

class MemBufTest : public testing::Test {};

TEST(MemBufTest, membuf_release_success) {
  ASSERT_EQ(MemBuf().IsFree(), true);
}
