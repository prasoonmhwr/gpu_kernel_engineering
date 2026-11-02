// common.h: tiny helpers shared by all demos
#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static inline double now(void) {            // seconds, monotonic clock
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

// Compiler barrier: stops the compiler from hoisting repeated work out of a
// benchmark loop. Works on GCC and Clang, x86 and Arm.
#define BARRIER() __asm__ volatile("" ::: "memory")

// Cache-line padding in floats. 32 floats = 128 bytes, which covers both the
// 64-byte lines of x86 and the 128-byte lines of Apple M-series chips.
#define PAD_FLOATS 32
#endif
