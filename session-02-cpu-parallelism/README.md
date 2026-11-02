# Session 2 demos: CPU parallelism

Seven small C programs, one per demo in the video. Works on Apple Silicon Macs,
Intel Macs and Linux.

## One-time setup on a Mac

```sh
xcode-select --install        # compiler (skip if you already have it)
brew install libomp           # OpenMP; Apple's compiler does not ship it
```

## Build and run

```sh
cd session-02-cpu-parallelism
make                          # builds everything
./run_all.sh                  # runs every demo in the order of the video
```

`make clean` removes the binaries. If `./run_all.sh` says "permission denied",
run `chmod +x run_all.sh info.sh` once.

## Demo by demo

| Part | Slide | Command |
|---|---|---|
| 2 | Random vs sorted | `./branch-O0 random` · `./branch-O0 sorted` · `./branch-O2 random` · `./branch-O2 sorted` |
| 3 | Accumulators | `./acc 1` · `./acc 2` · `./acc 4` · `./acc 8` · then the same with `--big` |
| 4 | SIMD | `./simd` |
| 5 | Threads | `sh info.sh` (your `lscpu`), then `OMP_NUM_THREADS=1 ./dot` and 2, 4, 8 |
| 5 | Data race | `./race` three times |
| 5 | False sharing | `OMP_NUM_THREADS=4 ./falseshare unpadded` · `padded` · `local` |
| 6 | Rows vs columns | `./sum rows` · `./sum cols` |

The slide commands (`gcc -O0 branch.c -o branch`, and so on) also work if you
prefer typing them. On a Mac, `gcc` is Apple's clang. Programs that use OpenMP
(`dot`, `race`, `falseshare`) need the extra flags the Makefile adds, so use
`make` for those:

```sh
clang -Xpreprocessor -fopenmp -I$(brew --prefix libomp)/include \
      -L$(brew --prefix libomp)/lib -lomp -O0 race.c -o race
```

## What to expect, and what differs on a Mac

- **Branch (Part 2):** at `-O0` sorted is clearly faster (about 3 to 4 times on my test machine). At `-O2` both are fast and equal, because the compiler removes the branch. That is the honest-note slide.
- **Accumulators (Part 3):** the gap between 1 and 4 accumulators depends on how long an add takes on your chip. It is large on Apple M-series, smaller on some recent Intel cores. Show whatever you get. `--big` should shrink the gap, because the loop becomes memory-bound.
- **SIMD (Part 4):** x86 uses AVX2 and FMA (8 floats per instruction). Apple Silicon uses NEON (4 floats per instruction). The same file handles both; it prints which one it used, so say "4 lanes on this Mac" on screen.
- **Threads (Part 5):** speed-up flattens early because the array is large and memory limits it. Apple M-series chips have performance and efficiency cores, so the curve can kink above the number of fast cores. `info.sh` shows both counts.
- **Race:** the printed total is below 1,000,000 and changes on every run. It is built with `-O0` on purpose.
- **False sharing:** padding is 128 bytes (32 floats), which covers the 128-byte cache lines of Apple chips and the 64-byte lines of x86. The gap needs threads on separate physical cores. On my two-vCPU test machine there was no gap, which is the case the script's honesty note covers. Expect a clear gap on a Mac with several performance cores. The `local` mode is the fastest and is the real fix.
- **Rows vs columns:** columns are several times slower. It is built at `-O1` so the compiler does not swap the loops.

## Other tips

- Close other apps and plug in power while timing. Run each demo two or three times and report the steady number.
- The programs print their own timings. Replace the shape-only bars in the slides with your numbers.
- Intel Mac without AVX2 (very old): remove `simd` from the `all:` line in the Makefile.
