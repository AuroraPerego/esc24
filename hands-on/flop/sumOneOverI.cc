#include <iostream>
#include <iomanip>

#define N 10'000'000

template<typename T>
constexpr T forward_sum()
{
  T sum = static_cast<T>(0.);
  for (int i = 1; i <= N; ++i)
    sum += static_cast<T>(1.) / static_cast<T>(i);
  return sum;
}

template<typename T>
constexpr T backward_sum()
{
  T sum = static_cast<T>(0.);
  for (int i = N; i > 0; --i)
    sum += static_cast<T>(1.) / static_cast<T>(i);
  return sum;
}

int main(){
  std::cout << std::setprecision (7) << forward_sum<float>() << " forward in float\n";
  std::cout << std::setprecision (7) << backward_sum<float>() << " backward in float\n";
  std::cout << std::setprecision (15) << forward_sum<double>() << " forward in double\n";
  std::cout << std::setprecision (15) << backward_sum<double>() << " backward in double\n";
}
