.PHONY: all clean

CXX := g++

all: test basetest

clean:
	rm -f test

stb:
	git clone https://github.com/nothings/stb.git

fmt:
	git clone https://github.com/fmtlib/fmt.git

test: test.cc Makefile stb fmt
	$(CXX) -std=c++20 -O3 -g -Istb -Ifmt/include -Wall -march=native $< -o $@ -ltbb

basetest: base_test.cc Makefile stb fmt
	$(CXX) -std=c++20 -O3 -g -Istb -Ifmt/include -Wall -march=native $< -o $@

newtest: new_test.cc Makefile stb fmt
	$(CXX) -std=c++20 -O0 -g -Istb -Ifmt/include -Wall -march=native $< -o $@ -ltbb

