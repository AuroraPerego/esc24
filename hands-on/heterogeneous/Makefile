.PHONY: all clean

CXX := g++
NVCC := nvcc
HIPCC := hipcc
ICPX := icpx

all: test testTBB testOMP testCuda testHip testCpuSycl testIntelGpu

clean:
	rm -f test testTBB testOMP testCuda testHip testCpuSycl testIntelGpu

alpaka:
	git clone https://github.com/alpaka-group/alpaka.git --single-branch --branch develop

stb:
	git clone https://github.com/nothings/stb.git

fmt:
	git clone https://github.com/fmtlib/fmt.git

boost:
	$(eval BOOST_TMP := $(shell mktemp -d))
	curl -L -s -S https://archives.boost.io/release/1.86.0/source/boost_1_86_0.tar.bz2 | tar xj -C $(BOOST_TMP)
	cd $(BOOST_TMP)/boost_1_86_0 && ./bootstrap.sh && ./b2 install --prefix=$@ --without-graph_parallel --without-mpi --without-python
	cp -r $(BOOST_TMP)/boost_1_86_0/boost .
	rm -rf $(BOOST_TMP)

IMG_BASE := .
ALPAKA := alpaka/include
BOOST := boost/include
TBB := /opt/intel/oneapi/tbb/latest/include

EXTERNAL := -isystem $(ALPAKA) -isystem $(BOOST) -isystem $(IMG_BASE)/stb -isystem $(IMG_BASE)/fmt/include
CXXFLAGS := -std=c++20 -O3 -Wall
NVCCFLAGS := -std=c++20 -O3

test: test.cc kernel.h Makefile stb fmt alpaka boost
	$(CXX) $(CXXFLAGS) $(EXTERNAL) -march=native  $< -o $@ -DALPAKA_ACC_CPU_B_SEQ_T_SEQ_ENABLED -DALPAKA_DISABLE_VENDOR_RNG

testTBB: test.cc kernel.h Makefile stb fmt alpaka boost
	$(CXX) $(CXXFLAGS) $(EXTERNAL) -isystem $(TBB) -ltbb $< -o $@ -DALPAKA_ACC_CPU_B_TBB_T_SEQ_ENABLED -DALPAKA_DISABLE_VENDOR_RNG

testOMP: test.cc kernel.h Makefile stb fmt alpaka boost
	$(CXX) $(CXXFLAGS) $(EXTERNAL) -fopenmp $< -o $@ -DALPAKA_ACC_CPU_B_OMP2_T_SEQ_ENABLED -DALPAKA_DISABLE_VENDOR_RNG

testCuda: test.cc kernel.h Makefile stb fmt alpaka boost
	$(NVCC) $(NVCCFLAGS) $(EXTERNAL) --expt-relaxed-constexpr -diag-suppress 20208 -x cu $< -o $@ -DALPAKA_ACC_GPU_CUDA_ENABLED -DALPAKA_DISABLE_VENDOR_RNG

testHip: test.cc kernel.h Makefile stb fmt alpaka boost
	$(HIPCC) $(CXXFLAGS) $(EXTERNAL)  $< -o $@ -DALPAKA_ACC_GPU_HIP_ENABLED -DALPAKA_DISABLE_VENDOR_RNG -frelaxed-template-template-args

testCpuSycl: test.cc kernel.h Makefile stb fmt alpaka boost
	$(ICPX) $(CXXFLAGS) $(EXTERNAL) -fsycl -fsycl-targets=spir64_x86_64 $< -o $@ -DALPAKA_ACC_SYCL_ENABLED -DALPAKA_SYCL_ONEAPI_CPU -DALPAKA_DISABLE_VENDOR_RNG

testIntelGpu: test.cc kernel.h Makefile stb fmt alpaka boost
	$(ICPX) $(CXXFLAGS) $(EXTERNAL) -fsycl -fsycl-targets=intel_gpu_pvc $< -o $@ -DALPAKA_ACC_SYCL_ENABLED -DALPAKA_SYCL_ONEAPI_GPU -DALPAKA_DISABLE_VENDOR_RNG
