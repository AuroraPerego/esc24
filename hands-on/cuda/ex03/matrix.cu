// C++ standard headers
#include <cassert>
#include <iostream>
#include <vector>

// CUDA headers
#include <cuda_runtime.h>

// local headers
#include "cuda_check.h"

// Here you can set the device ID that was assigned to you
#define MYDEVICE 0

// Part 2 of 4: implement the kernel
__global__ void kernel(int* a, int dimx, int dimy)
{
  auto const x = threadIdx.x + blockDim.x * blockIdx.x;
  auto const y = threadIdx.y + blockDim.y * blockIdx.y;
  if (x < dimx and y < dimy) {
    a[x+dimx*y] = x+dimx*y;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////
int main()
{
  CUDA_CHECK(cudaSetDevice(MYDEVICE));

  // Create a CUDA stream to execute asynchronous operations on this device
  cudaStream_t queue;
  CUDA_CHECK(cudaStreamCreate(&queue));

  // Part 1 and 4 of 4: set the dimensions of the matrix
  int dimx = 19;
  int dimy = 67;

  // Allocate enough memory on the host
  std::vector<int> h_a(dimx * dimy);
  int num_bytes = dimx * dimy * sizeof(int);

  // Pointer for the device memory
  int* d_a;

  // Allocate enough memory on the device
  CUDA_CHECK(cudaMallocAsync(&d_a, num_bytes, queue));

  // Part 2 of 4: define grid and block size and launch the kernel
  dim3 numberOfBlocks, numberOfThreadsPerBlock;
  numberOfThreadsPerBlock.x = 32;
  numberOfThreadsPerBlock.y = 32;
  numberOfBlocks.x  = (numberOfThreadsPerBlock.x+dimx-1)/numberOfThreadsPerBlock.x;
  numberOfBlocks.y  = (numberOfThreadsPerBlock.y+dimy-1)/numberOfThreadsPerBlock.y;

  kernel<<<numberOfBlocks, numberOfThreadsPerBlock, 0, queue>>>(d_a, dimx, dimy);
  CUDA_CHECK(cudaGetLastError());

  // Device to host copy
  CUDA_CHECK(cudaMemcpyAsync(h_a.data(), d_a, num_bytes, cudaMemcpyDeviceToHost, queue));

  // Free the device memory
  CUDA_CHECK(cudaFreeAsync(d_a, queue));


  // Wait for all asynchronous operations to complete
  CUDA_CHECK(cudaStreamSynchronize(queue));


  // verify the data returned to the host is correct
  for (int row = 0; row < dimy; ++row) {
    for (int col = 0; col < dimx; ++col) {
      assert(h_a[row * dimx + col] == row * dimx + col);
    }
  }

  // Destroy the CUDA stream
  CUDA_CHECK(cudaStreamDestroy(queue));

  // If the program makes it this far, then the results are correct and
  // there are no run-time errors.  Good work!
  std::cout << "Correct!" << std::endl;

  return 0;
}
