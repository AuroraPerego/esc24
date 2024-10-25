.PHONY: all clean

CXX := g++

all: test

clean:
	rm -f test

stb:
	git clone https://github.com/nothings/stb.git

fmt:
	git clone https://github.com/fmtlib/fmt.git

BASE := /home/HPC/student150/esc24/hands-on/tbb/04_images

test: test.cc Makefile stb fmt
	$(CXX) -std=c++20 -O3 -g -I$(BASE)/stb -I$(BASE)/fmt/include -Wall -march=native -ltbb $< -o $@

