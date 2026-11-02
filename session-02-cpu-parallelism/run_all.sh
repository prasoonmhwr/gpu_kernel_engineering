#!/bin/sh
# Runs every demo in the order of the video. Usage: ./run_all.sh
set -e
cd "$(dirname "$0")"
make -s
echo "=== machine ===";                 sh ./info.sh
echo; echo "=== Part 2: branch prediction ==="
echo "-- -O0 --"; ./branch-O0 random; ./branch-O0 sorted
echo "-- -O2 --"; ./branch-O2 random; ./branch-O2 sorted
echo; echo "=== Part 3: accumulators ==="
for k in 1 2 4 8; do ./acc $k; done
echo "-- big array --"
for k in 1 2 4 8; do ./acc $k --big; done
echo; echo "=== Part 4: SIMD ===";      ./simd
echo; echo "=== Part 5: threads ==="
for t in 1 2 4 8; do OMP_NUM_THREADS=$t ./dot; done
echo "-- data race (3 runs) --"; ./race; ./race; ./race
echo "-- false sharing --"
for m in unpadded padded local; do OMP_NUM_THREADS=4 ./falseshare $m; done
echo; echo "=== Part 6: rows versus columns ==="
./sum rows; ./sum cols
