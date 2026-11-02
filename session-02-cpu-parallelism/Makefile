# Session 2 demos. Works on macOS (Apple Silicon and Intel) and Linux.
#   make          build everything
#   make clean
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)
CC      ?= cc

# --- OpenMP -----------------------------------------------------------------
ifeq ($(UNAME_S),Darwin)
  # Apple's compiler has no OpenMP built in. `brew install libomp` provides it.
  LIBOMP := $(shell brew --prefix libomp 2>/dev/null)
  OMP     = -Xpreprocessor -fopenmp -I$(LIBOMP)/include -L$(LIBOMP)/lib -lomp
  OMPSIMD = -Xpreprocessor -fopenmp-simd
else
  OMP     = -fopenmp
  OMPSIMD = -fopenmp-simd
endif

# --- CPU-specific flags -----------------------------------------------------
ifeq ($(UNAME_M),x86_64)
  NATIVE   = -march=native
  SIMDARCH = -mavx2 -mfma          # needs an AVX2 + FMA CPU (any Intel/AMD since ~2015)
else
  NATIVE   =                       # Apple Silicon / Arm: NEON is always on, nothing to add
  SIMDARCH =                       # NEON is always on for arm64
endif

CFLAGS = -std=gnu11 -Wall -Wno-unknown-pragmas

# acc.c should keep exactly the accumulators we wrote, so switch off the
# compiler's own vectoriser for that file (flag names differ GCC vs Clang)
IS_CLANG := $(shell $(CC) --version 2>&1 | grep -ci clang)
ifeq ($(IS_CLANG),0)
  NOVEC = -fno-tree-vectorize
else
  NOVEC = -fno-vectorize -fno-slp-vectorize
endif

all: branch-O0 branch-O2 acc simd dot race falseshare sum

branch-O0: branch.c common.h
	$(CC) $(CFLAGS) -O0 branch.c -o $@
branch-O2: branch.c common.h
	$(CC) $(CFLAGS) -O2 branch.c -o $@
acc: acc.c common.h
	$(CC) $(CFLAGS) -O2 $(NOVEC) acc.c -o $@
simd: simd.c common.h
	$(CC) $(CFLAGS) -O2 $(SIMDARCH) $(OMPSIMD) simd.c -o $@
dot: dot.c common.h
	$(CC) $(CFLAGS) -O2 $(NATIVE) dot.c -o $@ $(OMP)
race: race.c
	$(CC) $(CFLAGS) -O0 race.c -o $@ $(OMP)
falseshare: falseshare.c common.h
	$(CC) $(CFLAGS) -O2 falseshare.c -o $@ $(OMP)
sum: sum.c common.h
	$(CC) $(CFLAGS) -O1 sum.c -o $@

clean:
	rm -f branch-O0 branch-O2 acc simd dot race falseshare sum
