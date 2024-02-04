#include "TccCalcTask.h"

namespace  tcc {

void TccCalcTask::CalcNonSparse(){
  partial_sum={};
    for (size_t m = 0; m != 32; ++m) {
      for (size_t k = 0; k != 32; ++k) {
        int8_t tmp = int8_t(ab[m][k]);
        for (size_t n = 0; n != 64; ++n) {
          partial_sum[m][n] += tmp * int8_t(wb[k][n]);
        }
      }
    }
}

}
