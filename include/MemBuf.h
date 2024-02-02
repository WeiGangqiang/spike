#ifndef INC_E240AA13337A4358B08F31978A37A835_H
#define INC_E240AA13337A4358B08F31978A37A835_H

#include  <stdint.h>

struct MemBuf {
    void AddRef() {
        ref_count++;
    }

    void RelRef() {
        ref_count--;
    }

    bool IsFree() {
        return ref_count == 0;
    }

private:
    uint64_t ref_count = 0;
};

#endif //INC_E240AA13337A4358B08F31978A37A835_H