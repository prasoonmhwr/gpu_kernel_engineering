// sum.c: Part 6 demo. Same numbers, same additions, different order.
// Build with -O1 or lower: at higher levels the compiler may swap the loops.
// Run: ./sum rows     ./sum cols
#include <string.h>
#include "common.h"

#define N 4096                                      // 4096 x 4096 floats = 64 MB

int main(int argc, char **argv) {
    if (argc < 2 || (strcmp(argv[1], "rows") && strcmp(argv[1], "cols"))) {
        fprintf(stderr, "usage: %s rows|cols\n", argv[0]);
        return 1;
    }
    float *m = malloc((size_t)N * N * sizeof(float));
    for (long i = 0; i < (long)N * N; i++) m[i] = 1.0f;

    int rows = !strcmp(argv[1], "rows");
    double sum = 0;
    double t0 = now();
    for (int rep = 0; rep < 5; rep++) {
        if (rows) {                                 // fast: walks memory in order
            for (int i = 0; i < N; i++)
                for (int j = 0; j < N; j++) sum += m[(size_t)i * N + j];
        } else {                                    // slow: jumps N floats each step
            for (int j = 0; j < N; j++)
                for (int i = 0; i < N; i++) sum += m[(size_t)i * N + j];
        }
    }
    double t1 = now();
    printf("%-4s  time %.3f s   (sum %.0f)\n", argv[1], t1 - t0, sum);
    return 0;
}
