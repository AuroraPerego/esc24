#include <iostream>
#include <iomanip>

constexpr double count(const double step, const int reps = 100000)
{
  double s = 0.f;
  for (int i = 0; i < reps; ++i)
    s += step;
  return s;
}

int main(){
  {
    constexpr double step = 1.f / 128.f;
    constexpr double result = count(step);
    std::cout << std::setprecision (15) << result << " s\n";
  }
  {
    constexpr double step = 10.f;
    constexpr double result = count(step);
    std::cout << std::setprecision (15) << result << " ms\n";
  }
}
