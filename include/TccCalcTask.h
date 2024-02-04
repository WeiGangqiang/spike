#ifndef INC_EFFD524CC8154E84AEF04BCADE2BFAB1_H
#define INC_EFFD524CC8154E84AEF04BCADE2BFAB1_H

#include <stdint.h>
#include <array>

namespace  tcc {
struct TccCalcTask {
  void CalcNonSparse();

  uint8_t * GetAbBy(size_t idx){
    return ab[idx].data();
  }
  uint8_t * GetWbBy(size_t idx){
    return wb[idx].data();
  }
  std::array<std::array<unsigned, 64>, 32>& GetSum(){
    return partial_sum;
  }

private:
  std::array<std::array<uint8_t, 64>, 32> ab;
  std::array<std::array<uint8_t, 64>, 32> wb;
  std::array<std::array<unsigned, 64>, 32> partial_sum;
};
}
#endif // INC_EFFD524CC8154E84AEF04BCADE2BFAB1_H