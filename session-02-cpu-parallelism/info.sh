#!/bin/sh
# info.sh: what the demos should say about your machine (the lscpu step).
if [ "$(uname -s)" = "Darwin" ]; then
  echo "Chip:              $(sysctl -n machdep.cpu.brand_string)"
  echo "Physical cores:    $(sysctl -n hw.physicalcpu)"
  echo "Logical cores:     $(sysctl -n hw.logicalcpu)   (more than physical means hyper-threading)"
  # Apple Silicon: performance and efficiency cores
  if sysctl -n hw.perflevel0.physicalcpu >/dev/null 2>&1; then
    echo "Fast (P) cores:    $(sysctl -n hw.perflevel0.physicalcpu)"
    echo "Slow (E) cores:    $(sysctl -n hw.perflevel1.physicalcpu)"
  fi
  echo "Cache line (bytes): $(sysctl -n hw.cachelinesize)"
  echo "L1d / L2 (bytes):  $(sysctl -n hw.l1dcachesize 2>/dev/null) / $(sysctl -n hw.l2cachesize 2>/dev/null)"
  echo "Memory (bytes):    $(sysctl -n hw.memsize)"
else
  lscpu | grep -E "Model name|^CPU\(s\)|Thread|Core|Socket|L1d|L2|L3"
fi
