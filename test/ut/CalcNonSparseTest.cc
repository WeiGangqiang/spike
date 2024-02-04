#include "MpmcQueue.h"
#include "gtest/gtest.h"
#include <array>
#include <hwy/highway.h>
using namespace  std;
namespace hn = hwy::HWY_NAMESPACE;

class CalcNonSparseTest : public testing::Test {};

TEST(CalcNonSparseTest, check_int8_convet) {
  uint8_t a = 0;
  int8_t b(a);
  ASSERT_EQ(b, 0);

  uint8_t c = 125;
  int8_t d(c);
  ASSERT_EQ(d, 125);

  uint8_t e = 255;
  int8_t f(e);
  int8_t *g = reinterpret_cast<int8_t *>(&e);
  ASSERT_EQ(f, -1);
  ASSERT_EQ(*g, -1);

  std::array<uint8_t, 10> arr_u;
  std::array<int8_t, 10> arr;

  arr_u.fill(-1);
  arr.fill(255);
  for(size_t i = 0; i< arr_u.size(); i++){
    ASSERT_EQ(arr_u[i], (uint8_t)(arr[i]));
  }
}

namespace {

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

int calculate_non_sparse_highway(array<array<uint8_t, 64>, 32> &ab,
                                 array<array<uint8_t, 64>, 32> &wb,
                                 array<array<int32_t, 64>, 32> &partial_sum) {
  for (size_t m = 0; m != 32; ++m) {
    for (size_t k = 0; k != 32; ++k) {
      array<int16_t, 16> temp;
      temp.fill(int8_t(ab[m][k]));
      HighwayMulAddIntel(temp.data(),  reinterpret_cast<int8_t *>(wb[k].data()), partial_sum[m].data());
      HighwayMulAddIntel(temp.data(),  reinterpret_cast<int8_t *>(wb[k].data() + 16), partial_sum[m].data() + 16);
      HighwayMulAddIntel(temp.data(),  reinterpret_cast<int8_t *>(wb[k].data() + 32), partial_sum[m].data() + 32);
      HighwayMulAddIntel(temp.data(),  reinterpret_cast<int8_t *>(wb[k].data() + 48), partial_sum[m].data() + 48);
    }
  }
  return 0;
}
//
//int calculate_non_sparse_highway_refact(array<array<uint8_t, 64>, 32> &ab,
//                                 array<array<uint8_t, 64>, 32> &wb_ori,
//                                 array<array<int32_t, 64>, 32> &partial_sum) {
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


bool CompareEqual(array<array<uint8_t, 64>, 32>& ab, array<array<uint8_t, 64>, 32>& wb){
  array<array<int32_t, 64>, 32> normalRet;
  array<array<int32_t, 64>, 32> highWayRet;
  normalRet = {{0}};
  highWayRet = {{0}};

  calculate_non_sparse(ab, wb, normalRet);
  calculate_non_sparse_highway(ab, wb, highWayRet);

  for(size_t i = 0; i < 32 ; i++){
    for(size_t j = 0; j< 64; j++){
      if(normalRet[i][j] != highWayRet[i][j]){
        printf("pos[%ld][%ld], value : %d  != %d \n", i, j, normalRet[i][j], highWayRet[i][j]);
        return false;
      }
    }
  }
  return true;
}

}


TEST(CalcNonSparseTest, highway_and_normal_compare){

    hn::ScalableTag<int32_t> d32;

    printf("lane 32 %d \n", hn::Lanes(d32));

    array<array<uint8_t, 64>, 32> ab;
    array<array<uint8_t, 64>, 32> wb;

    for(size_t i = 0; i< 32; i++){
      ab[i].fill(uint8_t(2));
      wb[i].fill(uint8_t(3));
    }

    ASSERT_TRUE(CompareEqual(ab, wb));

    for(size_t i = 0; i< 32; i++){
      ab[i].fill(uint8_t(25));
      wb[i].fill(uint8_t(99));
    }

    ASSERT_TRUE(CompareEqual(ab, wb));

    for(size_t i = 0; i< 32; i++){
      ab[i].fill(uint8_t(207));
      wb[i].fill(uint8_t(199));
    }

    ASSERT_TRUE(CompareEqual(ab, wb));


    for(size_t i = 0; i< 32; i++){
      ab[i].fill(uint8_t(101));
      wb[i].fill(uint8_t(199));
    }

    ASSERT_TRUE(CompareEqual(ab, wb));
}


