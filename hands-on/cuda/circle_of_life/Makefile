gif-h:
	git clone https://github.com/charlietangora/gif-h

serial: gif-h
	g++ -std=c++20 -O3 -Wall -isystem ../../heterogeneous/alpaka/include -isystem /usr/include/boost/include -I gif-h -march=native -DALPAKA_ACC_CPU_B_SEQ_T_SEQ_ENABLED -DALPAKA_DISABLE_VENDOR_RNG circle_of_life.cpp -o circle_of_life_serial

clean:
	rm -rf circle_of_life circle_of_life_cuda

cuda:
	nvcc circle_of_life.cu -o circle_of_life_cuda -O2 -std=c++20 -I gif-h

all: serial cuda

run: all
	./circle_of_life
	./circle_of_life_cuda

