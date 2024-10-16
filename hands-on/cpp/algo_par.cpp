#include <random>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <execution>
#include <chrono>
#include <cassert>

using Clock = std::chrono::steady_clock;
using Duration = std::chrono::duration<float>;

int main()
{
  // define a pseudo-random number generator engine and seed it using an actual
  // random device
  std::random_device rd;
  std::default_random_engine eng{rd()};

  int const MAX_N = 100;
  std::uniform_int_distribution<int> uniform_dist{1, MAX_N};

  // fill a vector with SIZE random numbers
  int const SIZE = 10'000'000;
  std::vector<int> v;
  v.reserve(SIZE);
  std::generate_n(std::back_inserter(v), SIZE, [&] { return uniform_dist(eng); });

  const int true_sum = std::accumulate(v.begin(), v.end(), 0);
  {
    float d{0};
    float var{0};
    for (int i = 0 ; i < 100; ++i){
    auto t0 = Clock::now();
    // sum all the elements of the vector with std::accumulate
    const int sum = std::accumulate(v.begin(), v.end(), 0);
    auto t1 = Clock::now();
    assert(sum == true_sum);
    d += (t1 - t0).count();
    var += (t1 - t0).count()*(t1 - t0).count();
    }
    auto mean = d / 100 * 1e-9;
    auto dev = std::sqrt(var / 100 - mean*mean) * 1e-9;
    std::cout << "accumulate in " << mean << " pm " << dev  << " s\n";
  }

  {
    auto t0 = Clock::now();
    // sum all the elements of the vector with std::reduce, sequential policy
    // NB you need to pass the initial value
    const int reduce = std::reduce(std::execution::seq, v.cbegin(), v.cend());
    auto t1 = Clock::now();
    assert(reduce == true_sum);
    Duration d = t1 - t0;
    std::cout << "reduce sequential in " << d.count() << " s\n";
  }

  {
    auto t0 = Clock::now();
    // sum all the elements of the vector with std::reduce, parallel policy
    // NB you need to pass the initial value
    const int reduce = std::reduce(std::execution::par, v.cbegin(), v.cend());
    auto t1 = Clock::now();
    assert(reduce == true_sum);
    Duration d = t1 - t0;
    std::cout << "reduce parallel in " << d.count() << " s\n";
  }

  {
    auto copy = v;
    auto t0 = Clock::now();
    // sort the vector with std::sort
    std::sort(copy.begin(), copy.end());
    auto t1 = Clock::now();
    Duration d = t1 - t0;
    std::cout << "sort in " << d.count() << " s\n";
  }

  {
    auto copy = v;
    auto t0 = Clock::now();
    // sort the vector with std::sort
    std::ranges::sort(copy);
    auto t1 = Clock::now();
    Duration d = t1 - t0;
    std::cout << "ranges::sort in " << d.count() << " s\n";
  }

  {
    auto copy = v;
    auto t0 = Clock::now();
    // sort the vector with std::sort, sequential policy
    std::sort(std::execution::seq, copy.begin(), copy.end());
    auto t1 = Clock::now();
    Duration d = t1 - t0;
    std::cout << "sort sequential in " << d.count() << " s\n";
  }

  {
    auto copy = v;
    auto t0 = Clock::now();
    // sort the vector with std::sort, parallel policy
    std::sort(std::execution::par, copy.begin(), copy.end());
    auto t1 = Clock::now();
    Duration d = t1 - t0;
    std::cout << "sort parallel in " << d.count() << " s\n";
  }
}
