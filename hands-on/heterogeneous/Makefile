.PHONY: all clean

CXX := g++
NVCC := nvcc
HIPCC := hipcc

all: test testTBB testCuda testHip

clean:
	rm -f test testTBB testCuda testHip

alpaka:
	git clone https://github.com/alpaka-group/alpaka.git --single-branch --branch develop

stb:
	git clone https://github.com/nothings/stb.git

fmt:
	git clone https://github.com/fmtlib/fmt.git

#todo boost
BOOST := /data/user/aperego/boost/include

IMG_BASE := .
ALPAKA := alpaka/include

test: test.cc Makefile alpaka kernel.h stb fmt
	$(CXX) -std=c++20 -O0 -g -isystem $(ALPAKA) -isystem $(BOOST) -isystem $(IMG_BASE)/stb -isystem $(IMG_BASE)/fmt/include -Wall -march=native  $< -o $@ -DALPAKA_ACC_CPU_B_SEQ_T_SEQ_ENABLED

testTBB: test.cc Makefile alpaka kernel.h stb fmt
	$(CXX) -std=c++20 -O3 -isystem $(ALPAKA) -isystem $(BOOST) -isystem $(IMG_BASE)/stb -isystem $(IMG_BASE)/fmt/include -Wall -isystem $(TBB)  $< -o $@ -DALPAKA_ACC_CPU_B_TBB_T_SEQ_ENABLED -ltbb -DALPAKA_DISABLE_VENDOR_RNG

testCuda: test.cc Makefile alpaka kernel.h stb fmt
	$(NVCC) -x cu -O0 -g -isystem $(ALPAKA) -isystem $(BOOST) -isystem $(IMG_BASE)/stb -isystem $(IMG_BASE)/fmt/include  $< -o $@ -DALPAKA_ACC_GPU_CUDA_ENABLED -ccbin $(CXX) --compiler-options '-std=c++20' --expt-relaxed-constexpr -DALPAKA_DISABLE_VENDOR_RNG

testHip: test.cc Makefile alpaka kernel.h stb fmt
	$(HIPCC) -O3 -std=c++20 -isystem $(ALPAKA) -isystem $(BOOST) -isystem $(IMG_BASE)/stb -isystem $(IMG_BASE)/fmt/include  $< -o $@ -DALPAKA_ACC_GPU_HIP_ENABLED -DALPAKA_DISABLE_VENDOR_RNG -frelaxed-template-template-args
