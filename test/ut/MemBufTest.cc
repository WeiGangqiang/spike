#include "MemBuf.h"
#include "gtest/gtest.h"
#include <hwy/highway.h>
#include <array>
using namespace  std;

namespace hn = hwy::HWY_NAMESPACE;

using T = float;

void MulAddLoop(const T* HWY_RESTRICT mul_array,
                const T* HWY_RESTRICT add_array,
                const size_t size, T* HWY_RESTRICT x_array) {
  const hn::ScalableTag<T> d;
  for (size_t i = 0; i < size; i += hn::Lanes(d)) {
    const auto mul = hn::Load(d, mul_array + i);
    const auto add = hn::Load(d, add_array + i);
    auto x = hn::Load(d, x_array + i);
    x = hn::MulAdd(mul, x, add);
    hn::Store(x, d, x_array + i);
  }
}


array<array<unsigned, 64>, 32> partial_sum;
array<array<uint8_t, 64>, 32> ab;
array<array<uint8_t, 64>, 32> wb;

void calculate_non_sparse() {
  for (size_t m = 0; m != 32; ++m) {
    for (size_t k = 0; k != 32; ++k) {
      int8_t tmp = int8_t(ab[m][k]);
      for (size_t n = 0; n != 64; ++n) {
        partial_sum[m][n] += tmp * int8_t(wb[k][n]);
      }
    }
  }
}

class MemBufTest : public testing::Test {};

TEST(MemBufTest, membuf_release_success) {
  ASSERT_EQ(MemBuf().IsFree(), true);
}
