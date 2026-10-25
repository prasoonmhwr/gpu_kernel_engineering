#include <cstdio>
#include <cstdlib>
#include <cuda_runtime.h>

// Stop with a readable message if any CUDA call fails.
#define CHECK(call)                                                      \
    do {                                                                 \
        cudaError_t e = (call);                                          \
        if (e != cudaSuccess) {                                          \
            printf("CUDA error: %s (line %d)\n",                         \
                   cudaGetErrorString(e), __LINE__);                     \
            exit(1);                                                     \
        }                                                                \
    } while (0)

__global__ void add_one(float *x, int n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) x[i] += 1.0f;
}

int main() {
    const int n = 1 << 20;                    // about a million numbers
    float *x = nullptr;
    CHECK(cudaMallocManaged(&x, n * sizeof(float))); // memory both CPU and GPU can see
    for (int i = 0; i < n; i++) x[i] = (float)i;

    int threads = 256;
    int blocks = (n + threads - 1) / threads;
    add_one<<<blocks, threads>>>(x, n);       // launch the kernel
    CHECK(cudaGetLastError());                // did the launch work?
    CHECK(cudaDeviceSynchronize());           // wait for the GPU to finish

    int bad = 0;
    for (int i = 0; i < n; i++)
        if (x[i] != (float)i + 1.0f) bad++;
    printf("%s (%d mismatches)\n", bad ? "FAIL" : "PASS", bad);

    CHECK(cudaFree(x));
    return 0;
}