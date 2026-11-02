// falseshare.c: Part 5 demo. False sharing.
//   ./falseshare unpadded   every thread's slot sits in the same cache line
//   ./falseshare padded     every slot gets a cache line to itself
//   ./falseshare local      keep a local total, write the shared slot once (best)
// Use 4 or more threads on separate physical cores:
//   OMP_NUM_THREADS=4 ./falseshare unpadded
// The shared slot is accessed through a volatile pointer so the compiler cannot
// keep it in a register. That reproduces what the hardware sees in the real case.
#include <omp.h>
#include <string.h>
#include "common.h"

#define ITERS 200000000L

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: %s unpadded|padded|local\n", argv[0]); return 1; }
    int mode = !strcmp(argv[1], "unpadded") ? 0 : !strcmp(argv[1], "padded") ? 1 : !strcmp(argv[1], "local") ? 2 : -1;
    if (mode < 0) { fprintf(stderr, "usage: %s unpadded|padded|local\n", argv[0]); return 1; }

    int stride = (mode == 0) ? 1 : PAD_FLOATS;      // distance between slots, in floats
    float *partial = aligned_alloc(128, 64 * PAD_FLOATS * sizeof(float));
    memset(partial, 0, 64 * PAD_FLOATS * sizeof(float));

    int threads = 1;
    double t0 = now();
    #pragma omp parallel
    {
        int t = omp_get_thread_num();
        #pragma omp single
        threads = omp_get_num_threads();
        volatile float *slot = &partial[t * stride];
        if (mode == 2) {
            float local = 0.0f;
            for (long i = 0; i < ITERS; i++) local += 1.0f;
            *slot = local;                          // one shared write at the end
        } else {
            for (long i = 0; i < ITERS; i++) *slot += 1.0f;   // shared write every time
        }
    }
    double t1 = now();
    printf("%-8s  %d threads  time %.3f s\n", argv[1], threads, t1 - t0);
    return 0;
}
