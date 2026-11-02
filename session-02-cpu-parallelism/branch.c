// branch.c: Part 2 demo. Same work, sorted versus random data.
// Build:  gcc -O0 branch.c -o branch      (the gap is large)
//         gcc -O2 branch.c -o branch      (the gap mostly disappears: the
//                                          compiler removes the branch)
// Run:    ./branch random     ./branch sorted
#include <string.h>
#include "common.h"

#define N 32768
#define REPS 10000

static int cmp(const void *a, const void *b) { return *(const int *)a - *(const int *)b; }

int main(int argc, char **argv) {
    if (argc < 2 || (strcmp(argv[1], "random") && strcmp(argv[1], "sorted"))) {
        fprintf(stderr, "usage: %s random|sorted\n", argv[0]);
        return 1;
    }
    static int x[N];
    srand(12345);
    for (int i = 0; i < N; i++) x[i] = rand() % 256;
    if (!strcmp(argv[1], "sorted")) qsort(x, N, sizeof(int), cmp);

    long long sum = 0;
    double t0 = now();
    for (int r = 0; r < REPS; r++) {
        for (int i = 0; i < N; i++)
            if (x[i] >= 128)          // unpredictable if random, predictable if sorted
                sum += x[i];
    }
    double t1 = now();
    printf("%-6s  time %.3f s   (sum %lld)\n", argv[1], t1 - t0, sum);
    return 0;
}
