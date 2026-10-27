# GPU Kernel Engineering

A free, code-first video series on writing fast GPU kernels, from "what is a GPU" to FlashAttention and the newest NVIDIA hardware. 44 sessions. Every session pairs one concept with code: we explain the idea, write it live, profile it, and check whether it actually got faster.

**Watch:** [Playlist](https://www.youtube.com/playlist?list=PLBBEkEiy4Rx4) · [Intro and roadmap video](https://youtu.be/cibk7G-jHQw)

---

## Why this exists

- **Speed and cost.** A well-written kernel can be several times faster than a naive one, and on an expensive GPU that difference is real money.
- **Frameworks hide kernels.** You can use PyTorch for years and never see one, which is fine until something is slow and you have no idea why. Kernel engineering is how you find out.
- **The material is scattered.** Good blog posts, papers and lectures exist, but each assumes you already know the previous step. This series is meant to be one free path through all of it.

## Who it is for

You do not need to know CUDA. You do need:

| You need | How much |
|---|---|
| C / C++ | Pointers, arrays, memory, compiling a multi-file program |
| Linear algebra | Matrix multiplication, shapes, dot products |
| PyTorch | Tensors, devices, timing a matmul |
| An NVIDIA GPU | Your own, a free notebook, or rented by the hou |


## What we cover

1. **Foundations:** CPU parallelism, how a GPU works, the roofline model, first CUDA kernels
2. **Memory:** on a GPU, moving data is usually the real bottleneck
3. **GEMM:** matrix multiplication from a naive kernel to an optimized one, profiling at every step
4. **Tensor cores:** the dedicated matrix hardware on modern GPUs
5. **Profiling and debugging:** find out why a kernel is slow instead of guessing
6. **PyTorch integration and Triton:** connect your kernels to PyTorch and write your first Triton kernels
7. **Attention:** build FlashAttention live
8. **The modern frontier:** H100 features (TMA, WGMMA), Triton, CUTLASS, inference-serving kernels, FP8 and quantization, multi-GPU basics, Blackwell and NVFP4, FlashAttention 4, DeepSeek's kernels, AI-written kernels, and a capstone project


## Hardware

Most of the series runs on any NVIDIA GPU your CUDA toolkit supports. Some sessions need more:

| Sessions | What you need |
|---|---|
| CUDA basics, memory, profiling, early GEMM | Any NVIDIA GPU supported by your toolkit (Turing or newer for CUDA 13; older cards can use a CUDA 12 toolkit) |
| Tensor core sessions | A GPU with tensor cores: RTX 20-series or newer, T4, or a data-center card. GTX 16-series has none. |
| Ampere-specific instructions | Ampere or newer (RTX 30-series, A10, A100, and later) |
| FP8 hands-on | Ada or Hopper |
| Hopper sessions (TMA, WGMMA) | An H100. Most people will rent one for a few hours. |
| Blackwell sessions | Explanation only. No hardware needed to follow. |

No GPU at home? Free notebooks work for the early sessions, and renting a cheap Ampere-class card by the hour covers most of the rest. Session 1 compares the options. Before you commit to a provider, check that Nsight Compute can read performance counters there.

## Quick start

1. Check your GPU and its compute capability:

   ```bash
   nvidia-smi
   nvidia-smi --query-gpu=name,compute_cap --format=csv
   ```

2. Install the CUDA toolkit using the selector on NVIDIA's downloads page, then confirm the compiler:

   ```bash
   nvcc --version
   ```

3. Clone this repo and run the smoke test from Session 1:

   ```bash
   git clone REPO_URL
   cd REPO_NAME/session-01-prerequisites-setup
   nvcc -arch=native -o hello hello.cu
   ./hello
   ```

   You should see `PASS (0 mismatches)`.

4. Check that both profilers work on it:

   ```bash
   nsys profile -o hello_timeline ./hello
   ncu ./hello
   ```

   If `ncu` fails with `ERR_NVGPUCTRPERM`, GPU performance counters are not readable by your user. Session 1 covers the fix.

## Repository layout

```
.
├── README.md
├── session-01-prerequisites-setup/
│   ├── hello.cu        # smoke test: add-one kernel with error checking
│   ├── bench.py        # correct GPU timing with CUDA events
│  
├── session-02-.../
└── ...
```

## How to follow along

- **Write the code yourself.** This skill only sticks when you type it.
- **Run it, then profile it.** Every session is a loop: run, profile, read what the profiler says, change one thing, run again.
- **Keep your own baseline.** Session 1 has you record your own matmul throughput. Later sessions compare against it.


## Author

Made by [@Prasoon](https://github.com/prasoonmhwr). If you find it useful, subscribe to the channel, save the playlist, and share it with a friend who keeps saying they want to learn CUDA.
