#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <execution>
#include <iostream>
#include <random>
#include <vector>

bool is_sorted(std::vector<std::uint64_t> const& v) {
  if (v.empty()) {
    return true;
  }

  for (size_t i = 1; i < v.size(); ++i) {
    if (v[i] < v[i - 1])
      return false;
  }

  return true;
}

void measure(std::vector<std::uint64_t> v, bool verbose=false) {
  if (verbose) {
# if defined(SEQ)
  std::cout << "sequential sort\n";
# elif defined(PAR)
  std::cout << "parallel sort\n";
#elif defined(PAR_UNSEQ)
  std::cout << "parallel unsequenced sort\n";
#elif defined(UNSEQ)
  std::cout << "unsequenced sort\n";
#else
  std::cout << "ranges sort\n";
#endif
  }
  const auto start = std::chrono::steady_clock::now();
# if defined(SEQ)
  std::sort(std::execution::seq, v.begin(), v.end());
# elif defined(PAR)
  std::sort(std::execution::par, v.begin(), v.end());
#elif defined(PAR_UNSEQ)
  std::sort(std::execution::par_unseq, v.begin(), v.end());
#elif defined(UNSEQ)
  std::sort(std::execution::unseq, v.begin(), v.end());
#else
  std::ranges::sort(v);
#endif
  const auto finish = std::chrono::steady_clock::now();
  if (verbose) {
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << " ms\n";
  }
  assert(is_sorted(v));
};

void repeat(std::vector<std::uint64_t> const& v, size_t times, size_t skip = 0) {
  for (size_t i = 0; i < skip; ++i) {
    measure(v);
  }
  for (size_t i = 0; i < times; ++i) {
    measure(v, true);
  }
}

int main() {
  const std::size_t size = 1'000'000;
  const std::size_t skip = 1;
  const std::size_t repeats = 10;

  std::vector<std::uint64_t> v(size);
  std::mt19937 gen{std::random_device{}()};
  std::ranges::generate(v, gen);

  repeat(v, repeats, skip);
  std::cout << '\n';

  // TODO
  //   - change the sequential sort to use std::execution::seq
  //   - try the other execution policies (par, unseq, par_unseq)
}
