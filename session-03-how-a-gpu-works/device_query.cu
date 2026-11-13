#include <cstdio>
#include <cuda_runtime.h>

int main() {
    int dev = 0;
    cudaGetDevice(&dev);

    cudaDeviceProp p;
    cudaError_t err = cudaGetDeviceProperties(&p, dev);
    if (err != cudaSuccess) {
        printf("CUDA error: %s\n", cudaGetErrorString(err));
        return 1;
    }

    int smClockKHz = 0, memClockKHz = 0;
    cudaDeviceGetAttribute(&smClockKHz,  cudaDevAttrClockRate, dev);
    cudaDeviceGetAttribute(&memClockKHz, cudaDevAttrMemoryClockRate, dev);

    // FP32 lanes per SM depend on the architecture (CUDA Best Practices throughput table):
    // 64 on Volta/Turing (7.x) and the A100 (8.0), 128 on the later chips we cover.
    int lanes = (p.major == 7 || (p.major == 8 && p.minor == 0)) ? 64 : 128;

    double tflops = 2.0 * p.multiProcessorCount * lanes
                    * (smClockKHz * 1e3) / 1e12;
    double gbps   = 2.0 * (memClockKHz * 1e3) * (p.memoryBusWidth / 8.0) / 1e9;

    printf("GPU:                     %s (cc %d.%d)\n", p.name, p.major, p.minor);
    printf("SMs:                     %d\n", p.multiProcessorCount);
    printf("Warp size:               %d\n", p.warpSize);
    printf("Max threads / block:     %d\n", p.maxThreadsPerBlock);
    printf("Max threads / SM:        %d\n", p.maxThreadsPerMultiProcessor);
    printf("Registers / SM:          %d\n", p.regsPerMultiprocessor);
    printf("Shared mem / SM:         %zu KB\n", p.sharedMemPerMultiprocessor / 1024);
    printf("Shared mem / block (opt-in): %zu KB\n", p.sharedMemPerBlockOptin / 1024);
    printf("L2 cache:                %d MB\n", p.l2CacheSize / (1024 * 1024));
    printf("Memory bus width:        %d bits\n", p.memoryBusWidth);
    printf("Memory:                  %.1f GB\n", p.totalGlobalMem / 1e9);
    printf("Peak FP32 (derived):     %.1f TFLOPS\n", tflops);
    printf("Peak bandwidth (derived):%.0f GB/s\n", gbps);
    return 0;
}