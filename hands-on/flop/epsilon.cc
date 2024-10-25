#include <iostream>
#include <iomanip>

template<typename T>
constexpr T find_eps()
{
  const T target = static_cast<T>(0.);
  T eps = static_cast<T>(1.);
  T const uno = static_cast<T>(1.);
  while (uno + eps != uno)
    eps /= static_cast<T>(2.);
  return eps;
}

int main(){
  {
    std::cout << std::setprecision (7) << find_eps<float>() << " in float\n";
  }
  {
    std::cout << std::setprecision (15) << find_eps<double>() << " in double\n";
  }
}
