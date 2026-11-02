// dot.c: Part 5 demo. Threaded dot product with OpenMP.
// Run with different thread counts:
//   OMP_NUM_THREADS=1 ./dot     OMP_NUM_THREADS=2 ./dot
//   OMP_NUM_THREADS=4 ./dot     OMP_NUM_THREADS=8 ./dot
#include <omp.h>
#include "common.h"

int main(void) {
    long n = 1L << 24, reps = 20;                  // 64 MB per array
    float *a = malloc(n * sizeof(float)), *b = malloc(n * sizeof(float));
    for (long i = 0; i < n; i++) { a[i] = 1.0f; b[i] = 0.001f; }

    int threads = 1;
    #pragma omp parallel
    #pragma omp single
    threads = omp_get_num_threads();

    float result = 0;
    double t0 = now();
    for (long r = 0; r < reps; r++) {
        float sum = 0.0f;
        #pragma omp parallel for simd reduction(+:sum)
        for (long i = 0; i < n; i++) sum += a[i] * b[i];
        result = sum; BARRIER();
    }
    double t1 = now();
    printf("%2d thread%s  time %.3f s   %.2f GFLOP/s   (result %.1f)\n",
           threads, threads == 1 ? " " : "s", t1 - t0, 2.0 * n * reps / (t1 - t0) / 1e9, result);
    return 0;
}
