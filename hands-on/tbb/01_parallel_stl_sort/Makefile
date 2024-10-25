.PHONY: all clean

CXX := g++

all: test testSeq testPar testParUnseq testUnseq

clean:
	rm -f test

test: test.cc Makefile
	$(CXX) -std=c++20 -O3 -g -Wall -march=native $< -o $@

testSeq: test.cc Makefile
	$(CXX) -ltbb -DSEQ -std=c++20 -O3 -g -Wall -march=native $< -o $@

testPar: test.cc Makefile
	$(CXX) -ltbb -DPAR -std=c++20 -O3 -g -Wall -march=native $< -o $@

testParUnseq: test.cc Makefile
	$(CXX) -ltbb -DPAR_UNSEQ -std=c++20 -O3 -g -Wall -march=native $< -o $@

testUnseq: test.cc Makefile
	$(CXX) -ltbb -DUNSEQ -std=c++20 -O3 -g -Wall -march=native $< -o $@
