#include <benchmark/benchmark.h>

#define TILE_NUM 32
#define IH 400
#define IW 200
#define IC 256
using fp8 = uint8_t;

const float fp8Scale = 1.1349938597997031;
const float int8Scale = 0.7288292784047062;

void ScaleFP8(fp8 *input_data, int N, int C, int H, int W, float scale) {
  for (int n = 0; n < N; ++n) {
    for (int h = 0; h < H; ++h) {
      for (int w = 0; w < W; ++w) {
        for (int c = 0; c < C; ++c) {
          int idx = (n * H * W + h * W + w) * C + c;
          input_data[idx] = float(input_data[idx]) * scale;
        }
      }
    }
  }
}
constexpr size_t tensorSize = IH * IW * IC;
uint8_t inputData[tensorSize];  
uint8_t dstData[tensorSize];

static void BM_scaleFp8(benchmark::State& state) {
  memset(inputData, 0x0, tensorSize);
  for (auto _ : state) {
    ScaleFP8(inputData,1, IC, IH, IW, fp8Scale);
  }
}

BENCHMARK(BM_scaleFp8);

void ScaleFP8_Opt(fp8 *input_data)
{
    constexpr size_t size = IC * IH * IW * 1;
    constexpr float scale = 1.1349938597997031;
    for (int idx = 0; idx < size; idx++)
    {
        input_data[idx] = float(input_data[idx]) * scale;
    }
}

static void BM_scaleFp8_Opt(benchmark::State& state) {
  memset(inputData, 0x0, tensorSize);
  for (auto _ : state) {
    ScaleFP8_Opt(inputData);
  }
}

BENCHMARK(BM_scaleFp8_Opt);

void ScaleFP8_Opt2(fp8 *input_data)
{
    // constexpr size_t size = IC * IH * IW * 1;
    constexpr float scale = 1.1349938597997031;
    for (int loop = 0; loop < IH * IW * 2; loop++)
    {
        for (int lanesize = 0; lanesize < 128; lanesize++)
        {
            auto idx = loop * 128 + lanesize;
            input_data[idx] = float(input_data[idx]) * scale;
        }
    }
}

static void BM_scaleFp8_Opt2(benchmark::State& state) {
  memset(inputData, 0x0, tensorSize);
  for (auto _ : state) {
    ScaleFP8_Opt2(inputData);
  }
}

BENCHMARK(BM_scaleFp8_Opt2);


static void BM_memcpy(benchmark::State& state) {
  for (auto _ : state) {
    memcpy(inputData, dstData, tensorSize);
  }
}

BENCHMARK(BM_memcpy);