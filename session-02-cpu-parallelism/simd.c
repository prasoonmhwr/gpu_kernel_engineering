// simd.c: Part 4 demo. The same dot product four ways.
//   scalar        plain loop, one accumulator
//   omp simd      same loop plus "#pragma omp simd reduction(+:sum)"
//   vec x1        hand-written vector intrinsics, one vector accumulator
//   vec x4        hand-written vector intrinsics, four vector accumulators
// Works on x86 (AVX2 + FMA, 8 floats) and on Apple Silicon / Arm (NEON, 4 floats).
// Build: see the Makefile.   Run: ./simd
#include "common.h"

#if defined(__AVX2__) && defined(__FMA__)
  #include <immintrin.h>
  #define LANES 8
  #define ISA "AVX2+FMA, 8 lanes"
  typedef __m256 vf;
  #define VZERO()        _mm256_setzero_ps()
  #define VLOAD(p)       _mm256_loadu_ps(p)
  #define VFMA(a, b, c)  _mm256_fmadd_ps(a, b, c)
  #define VADD(a, b)     _mm256_add_ps(a, b)
  static inline float vsum(vf v) {
      float t[8]; _mm256_storeu_ps(t, v);
      return ((t[0]+t[1])+(t[2]+t[3])) + ((t[4]+t[5])+(t[6]+t[7]));
  }
#elif defined(__ARM_NEON)
  #include <arm_neon.h>
  #define LANES 4
  #define ISA "NEON, 4 lanes"
  typedef float32x4_t vf;
  #define VZERO()        vdupq_n_f32(0.0f)
  #define VLOAD(p)       vld1q_f32(p)
  #define VFMA(a, b, c)  vfmaq_f32(c, a, b)
  #define VADD(a, b)     vaddq_f32(a, b)
  static inline float vsum(vf v) { return vaddvq_f32(v); }
#else
  #error "Build with -mavx2 -mfma (x86) or on an Arm machine with NEON"
#endif

#define NOINLINE __attribute__((noinline))

NOINLINE static float dot_scalar(const float *a, const float *b, long n) {
    float s = 0;
    for (long i = 0; i < n; i++) s += a[i] * b[i];
    return s;
}
NOINLINE static float dot_ompsimd(const float *a, const float *b, long n) {
    float s = 0;
    #pragma omp simd reduction(+:s)
    for (long i = 0; i < n; i++) s += a[i] * b[i];
    return s;
}
NOINLINE static float dot_vec1(const float *a, const float *b, long n) {
    vf acc = VZERO();
    long i = 0;
    for (; i + LANES <= n; i += LANES) acc = VFMA(VLOAD(a + i), VLOAD(b + i), acc);
    float s = vsum(acc);
    for (; i < n; i++) s += a[i] * b[i];          // the tail
    return s;
}
NOINLINE static float dot_vec4(const float *a, const float *b, long n) {
    vf a0 = VZERO(), a1 = VZERO(), a2 = VZERO(), a3 = VZERO();
    long i = 0;
    for (; i + 4 * LANES <= n; i += 4 * LANES) {
        a0 = VFMA(VLOAD(a + i),             VLOAD(b + i),             a0);
        a1 = VFMA(VLOAD(a + i + LANES),     VLOAD(b + i + LANES),     a1);
        a2 = VFMA(VLOAD(a + i + 2 * LANES), VLOAD(b + i + 2 * LANES), a2);
        a3 = VFMA(VLOAD(a + i + 3 * LANES), VLOAD(b + i + 3 * LANES), a3);
    }
    float s = vsum(VADD(VADD(a0, a1), VADD(a2, a3)));
    for (; i < n; i++) s += a[i] * b[i];
    return s;
}

int main(void) {
    long n = 1L << 14, reps = 40000;               // small array: stays in cache
    float *a = malloc(n * sizeof(float)), *b = malloc(n * sizeof(float));
    for (long i = 0; i < n; i++) { a[i] = 1.0f; b[i] = 0.001f; }

    struct { const char *name; float (*f)(const float *, const float *, long); } v[] = {
        {"scalar", dot_scalar}, {"omp simd", dot_ompsimd}, {"vec x1", dot_vec1}, {"vec x4", dot_vec4}};

    printf("%s\n", ISA);
    for (int k = 0; k < 4; k++) {
        volatile float sink = 0;
        double t0 = now();
        for (long r = 0; r < reps; r++) { sink += v[k].f(a, b, n); BARRIER(); }
        double t1 = now();
        printf("%-9s time %.3f s   %6.2f GFLOP/s   (result %.2f)\n",
               v[k].name, t1 - t0, 2.0 * n * reps / (t1 - t0) / 1e9, sink / reps);
    }
    return 0;
}
