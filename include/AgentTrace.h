#ifndef INC_6404B351779B44C3B2524A3723D86509_H
#define INC_6404B351779B44C3B2524A3723D86509_H
#include <chrono>
#include <iostream>

constexpr size_t MaxTraceCount = 2000;

struct AgentTrace {
    void Init(size_t  tile_id, size_t  AgentType){
      this->tile_id  = tile_id;
      this->AgentType = AgentType;
      this->state  = 0;
      this->idleIdx =0;
      this->activeIdx = 0;
    }
    void ToActive(){
      if(state == 0){
        auto time = std::chrono::system_clock::now().time_since_epoch();
        activeTime[activeIdx] = std::chrono::duration_cast<std::chrono::microseconds>(time).count();
        activeIdx = (activeIdx + 1) % MaxTraceCount;
        state = 1;
      }
    }

    void ToIdle(){
      if(state == 1){
        auto time = std::chrono::system_clock::now().time_since_epoch();
        IdleTime[idleIdx] = std::chrono::duration_cast<std::chrono::microseconds>(time).count();
        idleIdx = (idleIdx + 1) % MaxTraceCount;
        state = 0;
      }
    }
    void Dump() {
      if(activeIdx < 2){
        return;
      }
      std::cout << "Tile ID: " << tile_id << " AgentType: " << AgentType
                << std::endl;
      for (size_t i = 0; i < activeIdx; i++) {
        std::cout << " " << activeTime[i] << " ";
        if (i < idleIdx) {
          std::cout << IdleTime[i];
        }
        std::cout << std::endl;
      }
      std::cout << "=============================== \n";
    }

  private:
    size_t  tile_id;
    size_t  AgentType;
    uint64_t state;
    uint64_t activeTime[MaxTraceCount];
    uint64_t IdleTime[MaxTraceCount];
    size_t  activeIdx;
    size_t  idleIdx;
};

#endif // INC_6404B351779B44C3B2524A3723D86509_H