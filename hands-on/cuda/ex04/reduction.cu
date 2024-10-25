// C++ standard headers
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

// CUDA headers
#include <cuda_runtime.h>

// local headers
#include "cuda_check.h"

// Here you can set the device ID that was assigned to you
#define MYDEVICE 0

#define BLOCK_SiZE 1024

// Part 4 of 8: implement the kernel
__global__ void block_sum(const int* input,
                          int* per_block_results,
                          const size_t n)
{
  __shared__ int sdata[BLOCK_SiZE];

  auto gIdx = threadIdx.x + blockDim.x * blockIdx.x;
  auto lIdx = threadIdx.x;
  if (gIdx < n)
    sdata[lIdx] = input[gIdx];

  __syncthreads();

  for (int s = 1; s < blockDim.x; s *= 2) {
    auto id = 2*s*lIdx;
    if (id < blockDim.x)
      sdata[id] += sdata[id+s];
    __syncthreads();
  }

  if (lIdx == 0) atomicAdd(per_block_results, sdata[0]);
}

////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////
int main(void)
{
  std::random_device rd; // Will be used to obtain a seed for the random engine
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> distrib(-10, 10);
  // Create array of 256ki elements
  const int num_elements = 1 << 18;
  // Generate random input on the host
  std::vector<int> h_input(num_elements);
  for (auto& elt : h_input) {
    elt = distrib(gen);
  }

  int host_result = std::accumulate(h_input.begin(), h_input.end(), 0);
  std::cerr << "Host sum: " << host_result << std::endl;

  // Part 1 of 8: choose a device and create a CUDA stream
  CUDA_CHECK(cudaSetDevice(MYDEVICE));
  cudaStream_t q;
  CUDA_CHECK(cudaStreamCreate(&q));

  // Part 2 of 8: copy the input data to device memory
  int* d_input;
  CUDA_CHECK(cudaMallocAsync(&d_input, num_elements*sizeof(int), q));
  CUDA_CHECK(cudaMemcpyAsync(d_input, h_input.data(), num_elements*sizeof(int), cudaMemcpyHostToDevice, q));

  // Part 3 of 8: allocate memory for the partial sums
  // How much space does it need?
  int* device_result;
  CUDA_CHECK(cudaMallocAsync(&device_result, sizeof(int), q));
  CUDA_CHECK(cudaMemsetAsync(device_result, 0x00, sizeof(int), q));

  // Part 5 of 8: launch one kernel to compute, per-block, a partial sum.
  // How much shared memory does it need?
  int block_size = BLOCK_SiZE;
  int num_blocks = (num_elements + block_size - 1) / block_size;
  block_sum<<<num_blocks, block_size, 0, q>>>(d_input, device_result, num_elements);
  CUDA_CHECK(cudaGetLastError());

  // Part 7 of 8: copy the result back to the host
  host_result = 0;
  CUDA_CHECK(cudaMemcpyAsync(&host_result, device_result, sizeof(int), cudaMemcpyDeviceToHost, q));

  CUDA_CHECK(cudaStreamSynchronize(q));
  std::cout << "Device sum: " << host_result << std::endl;

  // Part 8 of 8: deallocate device memory and destroy the CUDA stream
  CUDA_CHECK(cudaFreeAsync(d_input, q));
  CUDA_CHECK(cudaFreeAsync(device_result, q));
  CUDA_CHECK(cudaStreamDestroy(q));

  return 0;
}
