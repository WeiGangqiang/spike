#ifndef INC_530750DA828A4369B8F7089438BA6422_H
#define INC_530750DA828A4369B8F7089438BA6422_H

#include <chrono>
#include <iostream>


struct StepTrace {
  void Begin(size_t idx){
    auto time = std::chrono::system_clock::now().time_since_epoch();
    start[idx]= std::chrono::duration_cast<std::chrono::microseconds>(time).count();
  }
  void End(size_t idx){
    auto time = std::chrono::system_clock::now().time_since_epoch();
    end[idx]= std::chrono::duration_cast<std::chrono::microseconds>(time).count();
  }

  void Dump(size_t max_idx) {
    for (size_t i = 0; i < max_idx; i++) {
      std::cout << " idx : " << i << " use time: " << (end[i] - start[i])
                << std::endl;
    }
  }

private:
  uint64_t start[20];
  uint64_t end[20];
};

#endif // INC_530750DA828A4369B8F7089438BA6422_H