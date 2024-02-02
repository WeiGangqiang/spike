#include <benchmark/benchmark.h>
#include "MemBuf.h"
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

static void BM_calculate_non_sparse(benchmark::State& state) {
  for(size_t i = 0; i< 32; i++){
    ab[i].fill(uint8_t(12));
    wb[i].fill(uint8_t(99));
  }
  for (auto _ : state) {
    calculate_non_sparse();
  }
}

BENCHMARK(BM_calculate_non_sparse);



static void BM_MemBufConstruct(benchmark::State &state) {
  for (auto _ : state) {
    MemBuf buf;
  }
}

BENCHMARK(BM_MemBufConstruct);