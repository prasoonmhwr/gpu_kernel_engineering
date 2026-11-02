// acc.c: Part 3 demo. One accumulator versus several, same arithmetic.
// Build:  gcc -O2 acc.c -o acc            (see the Makefile for the exact flags)
// Run:    ./acc 1      ./acc 2      ./acc 4      ./acc 8
//         ./acc 1 --big     (array too big for the cache: memory-bound,
//                            the speed-up shrinks)
#include <string.h>
#include "common.h"

// Each function does the same n multiplies and n adds. They differ only in
// how many independent running sums they keep. noinline keeps them separate.
#define NOINLINE __attribute__((noinline))

NOINLINE static float dot1(const float *a, const float *b, long n) {
    float s0 = 0;
    for (long i = 0; i < n; i++) s0 += a[i] * b[i];
    return s0;
}
NOINLINE static float dot2(const float *a, const float *b, long n) {
    float s0 = 0, s1 = 0;
    for (long i = 0; i < n; i += 2) { s0 += a[i] * b[i]; s1 += a[i+1] * b[i+1]; }
    return s0 + s1;
}
NOINLINE static float dot4(const float *a, const float *b, long n) {
    float s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    for (long i = 0; i < n; i += 4) {
        s0 += a[i]   * b[i];   s1 += a[i+1] * b[i+1];
        s2 += a[i+2] * b[i+2]; s3 += a[i+3] * b[i+3];
    }
    return (s0 + s1) + (s2 + s3);
}
NOINLINE static float dot8(const float *a, const float *b, long n) {
    float s0 = 0, s1 = 0, s2 = 0, s3 = 0, s4 = 0, s5 = 0, s6 = 0, s7 = 0;
    for (long i = 0; i < n; i += 8) {
        s0 += a[i]   * b[i];   s1 += a[i+1] * b[i+1];
        s2 += a[i+2] * b[i+2]; s3 += a[i+3] * b[i+3];
        s4 += a[i+4] * b[i+4]; s5 += a[i+5] * b[i+5];
        s6 += a[i+6] * b[i+6]; s7 += a[i+7] * b[i+7];
    }
    return ((s0 + s1) + (s2 + s3)) + ((s4 + s5) + (s6 + s7));
}

int main(int argc, char **argv) {
    int k = argc > 1 ? atoi(argv[1]) : 1;
    int big = argc > 2 && !strcmp(argv[2], "--big");
    if (k != 1 && k != 2 && k != 4 && k != 8) { fprintf(stderr, "usage: %s 1|2|4|8 [--big]\n", argv[0]); return 1; }

    long n    = big ? (1L << 25) : (1L << 14);   // 128 MB per array vs 64 KB per array
    long reps = big ? 10 : 40000;

    float *a = malloc(n * sizeof(float)), *b = malloc(n * sizeof(float));
    for (long i = 0; i < n; i++) { a[i] = 1.0f; b[i] = 0.001f; }

    float (*f)(const float *, const float *, long) =
        k == 1 ? dot1 : k == 2 ? dot2 : k == 4 ? dot4 : dot8;

    volatile float sink = 0;
    double t0 = now();
    for (long r = 0; r < reps; r++) { sink += f(a, b, n); BARRIER(); }
    double t1 = now();

    double gflops = 2.0 * n * reps / (t1 - t0) / 1e9;
    printf("%d accumulator%s  %s  time %.3f s   %.2f GFLOP/s   (result %.1f)\n",
           k, k == 1 ? " " : "s", big ? "big array  " : "small array", t1 - t0, gflops, sink);
    return 0;
}
