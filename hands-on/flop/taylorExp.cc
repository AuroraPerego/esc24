#include <iostream>
#include <iomanip>
#include <cmath>

#define N 20

constexpr long unsigned int factorial(const long unsigned int n)
{
    return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

//template <typename T>
//constexpr T ipow(T base, int exp)
//{
//    T result = 1.;
//    for (;;)
//    {
//        if (exp & 1)
//            result *= base;
//        exp >>= 1;
//        if (!exp)
//            break;
//        base *= base;
//    }
//
//    return result;
//}

template<typename T>
T compute_exp(const T x)
{
  T exp = static_cast<T>(1.);
  for (int n = 1 ; n < N ; ++n) {
    //std::cout << std::pow(x, n) << " " << (static_cast<T>(factorial(n))) << "\n";
    exp += std::pow(x, n) / (static_cast<T>(factorial(n)));
  }
  return exp;
}

int main(){
  {
    const float x = 3.f;
    std::cout << std::setprecision (7) << compute_exp<float>(x) << " in float against " << std::exp(x) << " from std::exp\n";
  }
  {
    const double x = -3.;
    std::cout << std::setprecision (15) << compute_exp<double>(x) << " in double against " << std::exp(x) << " from std::exp\n";
    std::cout << std::setprecision (15) << " 1/ e^x = " << 1. / compute_exp<double>(3.) << " vs "  << 1. / std::exp(3.) << "\n";
  }
}
