#include <benchmark/benchmark.h>
#include "MemBuf.h"

static void BM_MemBufConstruct(benchmark::State &state) {
  for (auto _ : state) {
    MemBuf buf;
  }
}

BENCHMARK(BM_MemBufConstruct);