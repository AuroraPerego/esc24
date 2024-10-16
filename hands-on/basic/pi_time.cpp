#include <cassert>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <utility>

using Duration = std::chrono::duration<float>;

constexpr std::pair<double, double> compute_pi(const int n)
{
  auto const step = 1. / n;
  auto sum = 0.;
  for (int i = 0; i != n; ++i) {
    auto x = (i + 0.5) * step;
    sum += 4. / (1. + x * x);
  }

  return {step, sum};
}

constexpr bool is_prime(const int n)
{
  for (int i = 2; i < n; ++i)
  {
    if (n%i==0) return false;
  }
  return true;
}

std::pair<double, Duration> pi(int n)
{
  assert(n > 0);

  auto const start = std::chrono::steady_clock::now();

  auto [step, sum] = compute_pi(n);

  auto const end = std::chrono::steady_clock::now();

  return {step * sum, end - start};
}

int main(int argc, char* argv[])
{
  int const n = (argc > 1) ? std::atoi(argv[1]) : 10;

  auto const [value, time] = pi(n);

  std::cout << "pi = " << value << " for " << n << " iterations"
            << " in " << time.count() << " s\n";
  std::cout << n << " is ";
  if (not is_prime(n)) std::cout << "not ";
  std::cout << "prime.\n";
}
