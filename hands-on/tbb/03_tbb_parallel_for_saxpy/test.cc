#include <chrono>
#include <cstdint>
//#include <format>
#include <iomanip>
#include <iostream>
#include <numbers>
#include <random>
#include <vector>

#include <tbb/tbb.h>

template <typename T>
void axpy(T a, T x, T y, T& z) {
  z = a * x + y;
}

template <typename T>
void sequential_axpy(T a, std::vector<T> const& x, std::vector<T> const& y, std::vector<T>& z) {
  std::size_t size = x.size();
  for (std::size_t i = 0; i < size; ++i) {
    axpy(a, x[i], y[i], z[i]);
  }
}

template <typename T>
void tbb_axpy(T a, std::vector<T> const& x, std::vector<T> const& y, std::vector<T>& z) {
  std::size_t size = x.size();
//               // id type  start  stop  step   lambda
//  tbb::parallel_for<size_t>( 0,   size,  1,  [&] (std::size_t i){
//    axpy(a, x[i], y[i], z[i]);
//  });

  tbb::parallel_for(
    tbb::blocked_range<std::size_t>{0, size, 64},
    [&] (tbb::blocked_range<std::size_t> const& range){
      //for (size_t i : range)
      for (auto i = range.begin(); i < range.end() ; ++i)
        axpy(a, x[i], y[i], z[i]);
    },
    tbb::simple_partitioner() // aware of cache, locality
    // tbb::static_partitioner()
  );
}

template <typename t>
void measure_sequential(t a, std::vector<t> const& x, std::vector<t> const& y) {
  std::vector<t> z(x.size(), 0);
  auto start = std::chrono::steady_clock::now();
  sequential_axpy(a, x, y, z);
  // tbb_axpy(a, x, y, z);
  auto finish = std::chrono::steady_clock::now();
  float ms = std::chrono::duration_cast<std::chrono::duration<float>>(finish - start).count() * 1000.f;
  //std::cout << std::format("{:6.1f}", ms) << " ms\n";
  std::cout << std::fixed << std::setprecision(1) << std::setw(6) << ms << " ms\n";
}

template <typename t>
void measure_parallel(t a, std::vector<t> const& x, std::vector<t> const& y) {
  std::vector<t> z(x.size(), 0);
  auto start = std::chrono::steady_clock::now();
  tbb_axpy(a, x, y, z);
  auto finish = std::chrono::steady_clock::now();
  float ms = std::chrono::duration_cast<std::chrono::duration<float>>(finish - start).count() * 1000.f;
  //std::cout << std::format("{:6.1f}", ms) << " ms\n";
  std::cout << std::fixed << std::setprecision(1) << std::setw(6) << ms << " ms\n";
}

int main() {
  const std::size_t size = 100'000'000;
  const std::size_t times = 10;

  std::mt19937 gen{std::random_device{}()};
  std::uniform_real_distribution<float> dis{-std::numbers::pi, std::numbers::pi};
  float a = dis(gen);
  std::vector<float> x(size);
  std::ranges::generate(x, [&] { return dis(gen); });
  std::vector<float> y(size);
  std::ranges::generate(y, [&] { return dis(gen); });

  std::cout << "sequential axpy\n";
  for (size_t i = 0; i < times; ++i)
    measure_sequential(a, x, y);
  std::cout << '\n';

  std::cout << "parallel axpy\n";
  for (size_t i = 0; i < times; ++i)
    measure_parallel(a, x, y);
  std::cout << '\n';

  // TODO
  //   - write a parallel version of sequential_axpy using tbb::parallel_for
  //   - measure its performance compared to sequential_axpy

}
