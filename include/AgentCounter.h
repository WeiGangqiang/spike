#ifndef INC_1D1E4598E2CB47019041B650D729BE37_H
#define INC_1D1E4598E2CB47019041B650D729BE37_H

#include <chrono>
#include <iostream>

struct AgentActivity {
  void Init(size_t  tile_id, size_t  AgentType){
    this->tile_id  = tile_id;
    this->AgentType = AgentType;
    this->ActiveCount = 0;
    this->IdleCount = 0;
    this->IdleTimeSpend = 0;
    this->ActiveTimeSpend = 0;
    this->state = 2;
    this->idle = std::chrono::high_resolution_clock::now();
    this->active = std::chrono::high_resolution_clock::now();
  }

  void ToActive(){
    if(state == 0){
      active = std::chrono::high_resolution_clock::now();
      state = 1;
      IdleCount ++ ;
      IdleTimeSpend += std::chrono::duration_cast<std::chrono::nanoseconds>(active - idle).count();
    }else if (state = 2){
      this->idle = std::chrono::high_resolution_clock::now();
      this->active = std::chrono::high_resolution_clock::now();
      state = 1;
    }
  }

  void ToIdle() {
    if (state == 1) {
      state = 0;
      idle = std::chrono::high_resolution_clock::now();
      ActiveCount++;
      ActiveTimeSpend +=
          std::chrono::duration_cast<std::chrono::nanoseconds>(idle - active)
              .count();
    }
  }

  void Dump(){
    std::cout<< "Tile ID: " << tile_id << " AgentType: "<< AgentType << std::endl;
    std::cout<< "Active Count: " << ActiveCount << "  Active Spend Time: "<< ActiveTimeSpend << std::endl;
    std::cout<< "Idle Count: " << IdleCount << "  Idle Spend Time: "<< IdleTimeSpend << std::endl;
  }

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> active;
  std::chrono::time_point<std::chrono::high_resolution_clock> idle;
  size_t  ActiveCount;
  size_t  IdleCount;
  size_t  ActiveTimeSpend;
  size_t  IdleTimeSpend;
  size_t  tile_id;
  size_t  AgentType;
  uint64_t state;
};

#endif // INC_1D1E4598E2CB47019041B650D729BE37_H