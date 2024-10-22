.PHONY: all clean

CXX := g++
NVCC := nvcc

all: test testCuda

clean:
	rm -f test

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

testCuda: test.cc Makefile alpaka kernel.h stb fmt
	$(NVCC) -x cu -O0 -g -isystem $(ALPAKA) -isystem $(BOOST) -isystem $(IMG_BASE)/stb -isystem $(IMG_BASE)/fmt/include  $< -o $@ -DALPAKA_ACC_GPU_CUDA_ENABLED -ccbin $(CXX) --compiler-options '-std=c++20' --expt-relaxed-constexpr -DALPAKA_DISABLE_VENDOR_RNG
