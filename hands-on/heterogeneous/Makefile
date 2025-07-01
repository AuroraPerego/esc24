.PHONY: all clean

CXX := g++
NVCC := nvcc
HIPCC := hipcc
ICPX := icpx

all: test testTBB testOMP testCuda testHip testCpuSycl testIntelGpu

clean:
	rm -f test testTBB testOMP testCuda testHip testCpuSycl testIntelGpu

stb:
	git clone https://github.com/nothings/stb.git

fmt:
	git clone https://github.com/fmtlib/fmt.git

IMG_BASE := .
ALPAKA := /opt/alpaka/include -DALPAKA_DISABLE_ATOMIC_ATOMICREF
TBB := /opt/miniforge3/include

EXTERNAL := -isystem $(ALPAKA) -isystem $(IMG_BASE)/stb -isystem $(IMG_BASE)/fmt/include
CXXFLAGS := -std=c++20 -O3 -Wall
NVCCFLAGS := -std=c++20 -O3

testSerial: test.cc kernel.h Makefile stb fmt
	$(CXX) $(CXXFLAGS) $(EXTERNAL) -march=native  $< -o $@ -DALPAKA_ACC_CPU_B_SEQ_T_SEQ_ENABLED -DALPAKA_DISABLE_VENDOR_RNG

testTBB: test.cc kernel.h Makefile stb fmt
	$(CXX) $(CXXFLAGS) $(EXTERNAL) -isystem $(TBB) -ltbb $< -o $@ -DALPAKA_ACC_CPU_B_TBB_T_SEQ_ENABLED -DALPAKA_DISABLE_VENDOR_RNG

testOMP: test.cc kernel.h Makefile stb fmt
	$(CXX) $(CXXFLAGS) $(EXTERNAL) -fopenmp $< -o $@ -DALPAKA_ACC_CPU_B_OMP2_T_SEQ_ENABLED -DALPAKA_DISABLE_VENDOR_RNG

testCuda: test.cc kernel.h Makefile stb fmt
	$(NVCC) $(NVCCFLAGS) $(EXTERNAL) --expt-relaxed-constexpr -diag-suppress 20208 -x cu $< -o $@ -DALPAKA_ACC_GPU_CUDA_ENABLED -DALPAKA_DISABLE_VENDOR_RNG

testHip: test.cc kernel.h Makefile stb fmt
	$(HIPCC) $(CXXFLAGS) $(EXTERNAL)  $< -o $@ -DALPAKA_ACC_GPU_HIP_ENABLED -DALPAKA_DISABLE_VENDOR_RNG -frelaxed-template-template-args

testCpuSycl: test.cc kernel.h Makefile stb fmt
	$(ICPX) $(CXXFLAGS) $(EXTERNAL) -fsycl -fsycl-targets=spir64_x86_64 $< -o $@ -DALPAKA_ACC_SYCL_ENABLED -DALPAKA_SYCL_ONEAPI_CPU -DALPAKA_DISABLE_VENDOR_RNG -DALPAKA_BLOCK_SHARED_DYN_MEMBER_ALLOC_KIB=0

testIntelGpu: test.cc kernel.h Makefile stb fmt
	$(ICPX) $(CXXFLAGS) $(EXTERNAL) -fsycl -fsycl-targets=intel_gpu_pvc $< -o $@ -DALPAKA_ACC_SYCL_ENABLED -DALPAKA_SYCL_ONEAPI_GPU -DALPAKA_DISABLE_VENDOR_RNG
