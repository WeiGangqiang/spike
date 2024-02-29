#include "MpmcQueue.h"
//#include <arm_neon.h>
#include <array>
#include <benchmark/benchmark.h>
#include <hwy/highway.h>
using namespace  std;
namespace hn = hwy::HWY_NAMESPACE;

int calculate_non_sparse(array<array<uint8_t, 64>, 32>& ab, array<array<uint8_t, 64>, 32>& wb, array<array<int32_t, 64>, 32>&partial_sum) {
  for (size_t m = 0; m != 32; ++m) {
    for (size_t k = 0; k != 32; ++k) {
      int8_t tmp = int8_t(ab[m][k]);
      for (size_t n = 0; n != 64; ++n) {
        partial_sum[m][n] += tmp * int8_t(wb[k][n]);
      }
    }
  }
  return 1;
}

static void BM_calculate_non_sparse(benchmark::State& state) {
  array<array<int32_t, 64>, 32> partial_sum;
  array<array<uint8_t, 64>, 32> ab;
  array<array<uint8_t, 64>, 32> wb;

  for(size_t i = 0; i< 32; i++){
    ab[i].fill(uint8_t(12));
    wb[i].fill(uint8_t(99));
    partial_sum[i].fill(0);
  }
  for (auto _ : state) {
    benchmark::DoNotOptimize(calculate_non_sparse(ab, wb, partial_sum));
  }
}

BENCHMARK(BM_calculate_non_sparse);

//
//inline void vmulAdd(int16x4_t& v_ab, int8_t* wb, int32_t* sum){
//  int8x8_t v_wb = vld1_s8(wb); // 加载wb
//  int16x8_t v_wb1= vmovl_s8(v_wb);
//  int32x4_t v_sum1 = vld1q_s32(sum);
//  int32x4_t v_sum2 = vld1q_s32(sum + 4);
//  v_sum1 = vmlal_s16(v_sum1, v_ab, vget_high_s16(v_wb1));
//  v_sum2 = vmlal_s16(v_sum1, v_ab, vget_low_s16(v_wb1));
//  vst1q_s32(sum, v_sum1);
//  vst1q_s32(sum + 4, v_sum2);
//}
//
//inline void HighwayMulAdd(const int16_t *ab, const int8_t *wb, int32_t *sum) {
//  hn::ScalableTag<int8_t> d8;
//  hn::ScalableTag<int16_t> d16;
//  hn::ScalableTag<int32_t> d32;
//  auto v_ab = hn::Load(d16, ab);
//  auto v_wb = hn::Load(d8, wb);
//
//  auto v_wb10 = hn::PromoteTo(d16, hn::UpperHalf(d8, v_wb));
//  auto v_wb01 = hn::PromoteTo(d16, hn::LowerHalf(v_wb));
//  auto mul10 = hn::Mul(v_ab, v_wb01);
//  auto mul01 = hn::Mul(v_ab, v_wb10);
//
//  auto v_sum1000 = hn::Load(d32, sum);
//  auto v_sum0100 = hn::Load(d32, sum + hn::Lanes(d32));
//  auto v_sum0010 = hn::Load(d32, sum + 2 * hn::Lanes(d32));
//  auto v_sum0001 = hn::Load(d32, sum + 3 * hn::Lanes(d32));
//  auto mul1000 = hn::PromoteTo(d32, hn::UpperHalf(d16, mul10));
//  auto mul0100 = hn::PromoteTo(d32, hn::LowerHalf(mul10));
//  auto mul0010 = hn::PromoteTo(d32, hn::UpperHalf(d16, mul01));
//  auto mul0001 = hn::PromoteTo(d32, hn::LowerHalf(mul01));
//
//  v_sum1000 = hn::Add(v_sum1000, mul1000);
//  v_sum0100 = hn::Add(v_sum0100, mul0100);
//  v_sum0010 = hn::Add(v_sum0010, mul0010);
//  v_sum0001 = hn::Add(v_sum0001, mul0001);
//
//  hn::Store(v_sum1000, d32, sum);
//  hn::Store(v_sum0100, d32, sum + hn::Lanes(d32));
//  hn::Store(v_sum0010, d32, sum + 2 * hn::Lanes(d32));
//  hn::Store(v_sum0001, d32, sum + 3 * hn::Lanes(d32));
//}

inline void HighwayMulAddIntel(const int16_t *ab, const int8_t *wb, int32_t *sum) {
  hn::ScalableTag<int8_t> d8;
  hn::ScalableTag<int16_t> d16;
  hn::ScalableTag<int32_t> d32;
  auto v_ab = hn::Load(d16, ab);
  auto v_wb = hn::Load(d8, wb);

  auto v_wb10 = hn::PromoteUpperTo(d16, v_wb);
  auto v_wb01 = hn::PromoteLowerTo(d16, v_wb);
  auto mul10 = hn::Mul(v_ab, v_wb01);
  auto mul01 = hn::Mul(v_ab, v_wb10);

  auto v_sum1000 = hn::Load(d32, sum);
  auto v_sum0100 = hn::Load(d32, sum + hn::Lanes(d32));
  auto v_sum0010 = hn::Load(d32, sum + 2 * hn::Lanes(d32));
  auto v_sum0001 = hn::Load(d32, sum + 3 * hn::Lanes(d32));
  auto mul1000 = hn::PromoteUpperTo(d32, mul10);
  auto mul0100 = hn::PromoteLowerTo(d32, mul10);
  auto mul0010 = hn::PromoteUpperTo(d32,mul01);
  auto mul0001 = hn::PromoteLowerTo(d32, mul01);

  v_sum1000 = hn::Add(v_sum1000, mul1000);
  v_sum0100 = hn::Add(v_sum0100, mul0100);
  v_sum0010 = hn::Add(v_sum0010, mul0010);
  v_sum0001 = hn::Add(v_sum0001, mul0001);

  hn::Store(v_sum1000, d32, sum);
  hn::Store(v_sum0100, d32, sum + hn::Lanes(d32));
  hn::Store(v_sum0010, d32, sum + 2 * hn::Lanes(d32));
  hn::Store(v_sum0001, d32, sum + 3 * hn::Lanes(d32));
}

int calculate_non_sparse_highway(array<array<uint8_t, 64>, 32> &ab,
                                 array<array<uint8_t, 64>, 32> &wb,
                                 array<array<int32_t, 64>, 32> &partial_sum) {
  for (size_t m = 0; m != 32; ++m) {
    for (size_t k = 0; k != 32; ++k) {
      alignas(32) array<int16_t, 16> temp;
      hn::ScalableTag<int8_t> d8;
      temp.fill(int8_t(ab[m][k]));
      HighwayMulAddIntel(temp.data(),  reinterpret_cast<int8_t *>(wb[k].data()), partial_sum[m].data());
//      HighwayMulAddIntel(temp.data(),  reinterpret_cast<int8_t *>(wb[k].data() + 16), partial_sum[m].data() + 16);
      HighwayMulAddIntel(temp.data(),  reinterpret_cast<int8_t *>(wb[k].data() + 32), partial_sum[m].data() + 32);
//      HighwayMulAddIntel(temp.data(),  reinterpret_cast<int8_t *>(wb[k].data() + 48), partial_sum[m].data() + 48);
    }
  }
  return 1;
}

static void BM_calculate_non_sparse_highway(benchmark::State& state) {
  alignas(32)  array<array<uint8_t, 64>, 32> ab;
  alignas(32)  array<array<uint8_t, 64>, 32> wb;
  alignas(32)  array<array<int32_t, 64>, 32> partial_sum;

  for(size_t i = 0; i< 32; i++){
    ab[i].fill(uint8_t(12));
    wb[i].fill(uint8_t(99));
    partial_sum[i].fill(0);
  }
  for (auto _ : state) {
    benchmark::DoNotOptimize(calculate_non_sparse_highway(ab, wb, partial_sum));
  }
}

BENCHMARK(BM_calculate_non_sparse_highway);


//
//int calculate_non_sparse_highway_refact(array<array<uint8_t, 64>, 32> &ab,
//                                        array<array<uint8_t, 64>, 32> &wb_ori,
//                                        array<array<int32_t, 64>, 32> &partial_sum) {
//  for (size_t m = 0; m != 32; ++m) {
//    for (size_t k = 0; k != 32; ++k) {
//      hn::ScalableTag<int8_t> d8;
//      hn::ScalableTag<int16_t> d16;
//      hn::ScalableTag<int32_t> d32;
//      array<int16_t, hn::Lanes(d8)> temp;
//      temp.fill(int8_t(ab[m][k]));
//      auto v_ab = hn::Load(d16, temp.data());
//      for (size_t n = 0; n < 64;  n = n + hn::Lanes(d8)){
//        auto wb = reinterpret_cast<int8_t *>(wb_ori[k].data() + n);
//        auto sum = partial_sum[m].data() + n;
//        auto v_wb = hn::Load(d8, wb);
//
//        auto v_wb10 = hn::PromoteTo(d16, hn::UpperHalf(d8, v_wb));
//        auto v_wb01 = hn::PromoteTo(d16, hn::LowerHalf(v_wb));
//        auto mul10 = hn::Mul(v_ab, v_wb01);
//        auto mul01 = hn::Mul(v_ab, v_wb10);
//
//        auto v_sum1000 = hn::Load(d32, sum);
//        auto v_sum0100 = hn::Load(d32, sum + hn::Lanes(d32));
//        auto v_sum0010 = hn::Load(d32, sum + 2 * hn::Lanes(d32));
//        auto v_sum0001 = hn::Load(d32, sum + 3 * hn::Lanes(d32));
//        auto mul1000 = hn::PromoteTo(d32, hn::UpperHalf(d16, mul10));
//        auto mul0100 = hn::PromoteTo(d32, hn::LowerHalf(mul10));
//        auto mul0010 = hn::PromoteTo(d32, hn::UpperHalf(d16, mul01));
//        auto mul0001 = hn::PromoteTo(d32, hn::LowerHalf(mul01));
//
//        v_sum1000 = hn::Add(v_sum1000, mul1000);
//        v_sum0100 = hn::Add(v_sum0100, mul0100);
//        v_sum0010 = hn::Add(v_sum0010, mul0010);
//        v_sum0001 = hn::Add(v_sum0001, mul0001);
//
//        hn::Store(v_sum1000, d32, sum);
//        hn::Store(v_sum0100, d32, sum + hn::Lanes(d32));
//        hn::Store(v_sum0010, d32, sum + 2 * hn::Lanes(d32));
//        hn::Store(v_sum0001, d32, sum + 3 * hn::Lanes(d32));
//      }
//    }
//  }
//}

//
//static void BM_calculate_non_sparse_highway_refact(benchmark::State& state) {
//  array<array<uint8_t, 64>, 32> ab;
//  array<array<uint8_t, 64>, 32> wb;
//  array<array<int32_t, 64>, 32> partial_sum;
//
//  for(size_t i = 0; i< 32; i++){
//    ab[i].fill(uint8_t(12));
//    wb[i].fill(uint8_t(99));
//    partial_sum[i].fill(0);
//  }
//  for (auto _ : state) {
//    benchmark::DoNotOptimize(calculate_non_sparse_highway_refact(ab, wb, partial_sum));
//  }
//}
//
//BENCHMARK(BM_calculate_non_sparse_highway_refact);



//
//int calculate_non_sparse_new(array<array<uint8_t, 64>, 32> &ab,
//                             array<array<uint8_t, 64>, 32> &wb,
//                             array<array<uint32_t, 64>, 32> &partial_sum) {
//  for (size_t m = 0; m != 32; ++m) {
//    for (size_t k = 0; k != 32; ++k) {
//      array<int16_t, 4> temp;
//      temp.fill(int16_t(ab[m][k]));
//      int16x4_t v_ab = vld1_s16(reinterpret_cast<const int16_t *>(temp.data())); // 加载wb
//      vmulAdd(v_ab, reinterpret_cast<int8_t *>(wb[k].data()), reinterpret_cast<int32_t *>(partial_sum[m].data()));
//      vmulAdd(v_ab, reinterpret_cast<int8_t *>(wb[k].data()+ 8), reinterpret_cast<int32_t *>(partial_sum[m].data() + 8));
//      vmulAdd(v_ab, reinterpret_cast<int8_t *>(wb[k].data()+ 16), reinterpret_cast<int32_t *>(partial_sum[m].data() + 16));
//      vmulAdd(v_ab, reinterpret_cast<int8_t *>(wb[k].data()+ 24), reinterpret_cast<int32_t *>(partial_sum[m].data() + 24));
//      vmulAdd(v_ab, reinterpret_cast<int8_t *>(wb[k].data()+ 32), reinterpret_cast<int32_t *>(partial_sum[m].data() + 32));
//      vmulAdd(v_ab, reinterpret_cast<int8_t *>(wb[k].data()+ 40), reinterpret_cast<int32_t *>(partial_sum[m].data() + 40));
//      vmulAdd(v_ab, reinterpret_cast<int8_t *>(wb[k].data()+ 48), reinterpret_cast<int32_t *>(partial_sum[m].data() + 48));
//      vmulAdd(v_ab, reinterpret_cast<int8_t *>(wb[k].data()+ 56), reinterpret_cast<int32_t *>(partial_sum[m].data() + 56));
//    }
//  }
//  return 1;
//}
//
//static void BM_calculate_non_sparse_new(benchmark::State& state) {
//  array<array<uint32_t, 64>, 32> partial_sum;
//  array<array<uint8_t, 64>, 32> ab;
//  array<array<uint8_t, 64>, 32> wb;
//
//  for(size_t i = 0; i< 32; i++){
//    ab[i].fill(uint8_t(12));
//    wb[i].fill(uint8_t(99));
//  }
//  for (auto _ : state) {
//    benchmark::DoNotOptimize(calculate_non_sparse_new(ab, wb, partial_sum));
//  }
//}

//BENCHMARK(BM_calculate_non_sparse_new);

//
//using IntArray = array<uint8_t, 64>;
//
//int normal_sum(IntArray& ab, IntArray& aw, IntArray & sum){
//  for(size_t i = 0; i< 64; i++){
//    sum[i]  += ab[i] * aw[i];
//  }
//  return 1;
//}
//
//int normal_sum_neon(IntArray & ab, IntArray & aw, IntArray& sum){
//  for(size_t i = 0; i< 64; i = i+16 ){
//    int8x16_t v_ab = vld1q_s8(reinterpret_cast<const int8_t*>(&ab[i])); // 加载wb
//    int8x16_t v_aw = vld1q_s8(reinterpret_cast<const int8_t*>(&aw[i])); // 加载wb
//    int8x16_t v_sum = vld1q_s8(reinterpret_cast<const int8_t*>(&sum[i])); // 加载wb
//    v_sum = vmlaq_s8(v_sum, v_ab, v_aw);
//    vst1q_s8(reinterpret_cast<int8_t*>(&sum[i]), v_sum);
//  }
//  return 1;
////}
////
////static void BM_normal_sum(benchmark::State& state) {
////  IntArray ab;
////  IntArray aw;
////  IntArray sum;
////  ab.fill(2);
////  aw.fill(4);
////  sum.fill(0);
////
////  for (auto _ : state) {
////    benchmark::DoNotOptimize(normal_sum(ab, aw, sum));
////  }
////}
////
////BENCHMARK(BM_normal_sum);
//
//

uint64_t dataCalloc = 0;

static void BM_calloc_free(benchmark::State &state) {
  auto PGSIZE = (1ull << 28);
  for (auto _ : state) {
    auto npuMem = (char *)calloc(PGSIZE, 1);
    dataCalloc += npuMem[10000];
    free(npuMem);
  }
}

BENCHMARK(BM_calloc_free);


uint64_t dataMalloc = 0;

static void BM_malloc_free(benchmark::State &state) {
  auto PGSIZE = (1ull << 28);
  for (auto _ : state) {
    auto npuMem = (char *)malloc(PGSIZE);
    dataMalloc += npuMem[10000];
    free(npuMem);
  }
}

BENCHMARK(BM_malloc_free);
