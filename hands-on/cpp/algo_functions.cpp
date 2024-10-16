#include <random>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <numeric>

typedef long int Type;
typedef float FType;

std::ostream& operator<<(std::ostream& os, std::vector<int> const& c);
std::vector<int> make_vector(int N);

int main()
{
  // create a vector of N elements, generated randomly
  int const N = 10;
  //std::vector<int> v = make_vector(N);
  std::vector<int> v(10);
  std::iota(v.begin(), v.end(), 1);
  std::cout << v << '\n';

  // multiply all the elements of the vector
  [[maybe_unused]] long int a = std::accumulate(v.begin(), v.end(), 1, std::multiplies<int>());
  // long int b = std::accumulate(v.begin(), v.end(), 1, [](int a, int b){return a*b;});

  // compute the mean and the standard deviation
  // use std::accumulate and a struct with two numbers to accumulate both the sum and the sum of squares
  std::pair<FType, FType> meanAndVar = std::accumulate(v.begin(), v.end(), std::pair<FType, FType>{0,0},
                                     [](std::pair<FType, FType> a, Type b){return std::pair<FType, FType>{a.first + b, a.second + b*b};} );
  std::cout << meanAndVar.first / N << " " << std::sqrt( meanAndVar.second / N - (meanAndVar.first / N)*(meanAndVar.first / N) ) << std::endl;

  // sort the vector in descending order
  std::ranges::sort(v, [](int a, int b){return a>b;});
  std::cout << v << '\n';

  // move the even numbers at the beginning of the vector
  std::ranges::partition(v, [](int a){return !(a%2);});
  std::cout << v << '\n';

  // create another vector with the squares of the numbers in the first vector
  {
    std::vector<int> squares;
    squares.reserve(N);
    std::transform(v.cbegin(), v.cend(), std::back_inserter(squares), [](int a){return a*a;});
    std::cout << squares << '\n';
  }
  {
    std::vector<int> squares(10);
    std::transform(v.cbegin(), v.cend(), squares.begin(), [](int a){return a*a;});
    std::cout << squares << '\n';
  }

  // find the first multiple of 3 or 7
  // use std::find_if
  {
    auto it = std::ranges::find_if(v, [](int a){ return (a%3 == 0) or (a%7 == 0);});
    std::cout << *it << '\n';
  }
  // erase from the vector all the multiples of 3 or 7
  // use std::remove_if followed by vector::erase
  {
    auto it = std::remove_if(v.begin(), v.end(), [](int a){ return (a%3 == 0) or (a%7 == 0);});
    v.erase(it, v.end());
    std::cout << v << '\n';
  }

  //   or the newer std::erase_if utility (C++20)
  {
    std::erase_if(v, [](int a){ return (a%3 == 0) or (a%7 == 0);});
    std::cout << v << '\n';
  }
}

std::ostream& operator<<(std::ostream& os, std::vector<int> const& c)
{
  os << "{ ";
  std::copy(
            std::begin(c),
            std::end(c),
            std::ostream_iterator<int>{os, " "}
            );
  os << '}';

  return os;
}

std::vector<int> make_vector(int N)
{
  // define a pseudo-random number generator engine and seed it using an actual
  // random device
  std::random_device rd;
  std::default_random_engine eng{rd()};

  int const MAX_N = 100;
  std::uniform_int_distribution<int> dist{1, MAX_N};

  std::vector<int> result;
  result.reserve(N);
  std::generate_n(std::back_inserter(result), N, [&] { return dist(eng); });

  return result;
}
