// Session 1 demo: the smallest useful CUDA program.
// Build:  nvcc -o my_program my_program.cu
// Run:    ./my_program
#include <cstdio>
#include <cuda_runtime.h>

#define CHECK(call)                                                        \
    do {                                                                   \
        cudaError_t err = (call);                                          \
        if (err != cudaSuccess) {                                          \
            fprintf(stderr, "CUDA error: %s (%s:%d)\n",                    \
                    cudaGetErrorString(err), __FILE__, __LINE__);          \
            return 1;                                                      \
        }                                                                  \
    } while (0)

__global__ void add_one(int *data, int n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) data[i] += 1;
}

int main() {
    cudaDeviceProp prop;
    CHECK(cudaGetDeviceProperties(&prop, 0));
    printf("GPU: %s (compute capability %d.%d)\n", prop.name, prop.major, prop.minor);

    const int n = 8;
    int host[n];
    for (int i = 0; i < n; i++) host[i] = i * 10;

    int *dev;
    CHECK(cudaMalloc(&dev, n * sizeof(int)));
    CHECK(cudaMemcpy(dev, host, n * sizeof(int), cudaMemcpyHostToDevice));

    add_one<<<1, n>>>(dev, n);
    CHECK(cudaGetLastError());
    CHECK(cudaDeviceSynchronize());

    CHECK(cudaMemcpy(host, dev, n * sizeof(int), cudaMemcpyDeviceToHost));
    CHECK(cudaFree(dev));

    printf("result:");
    for (int i = 0; i < n; i++) printf(" %d", host[i]);
    printf("\n");   // expected: 1 11 21 31 41 51 61 71
    return 0;
}
