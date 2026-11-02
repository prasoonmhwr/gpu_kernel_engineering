// race.c: Part 5 demo. A data race.
// Build with optimisation OFF on purpose (-O0): with it on, the compiler can
// fold the loop into one add per thread and the bug hides. It is still a bug.
// Run it three times: ./race ./race ./race
#include <omp.h>
#include <stdio.h>

int main(void) {
    int counter = 0;
    #pragma omp parallel for
    for (int i = 0; i < 1000000; i++) {
        counter++;                                  // read, add, write: three steps
    }
    printf("expected 1000000, got %d\n", counter);
    return 0;
}
