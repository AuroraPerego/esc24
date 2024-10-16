#include <random>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <numeric>

std::ostream& operator<<(std::ostream& os, std::vector<int> const& c);
std::vector<int> make_vector(int N);

int main()
{
  // create a vector of N elements, generated randomly
  int const N = 10;
  std::vector<int> v = make_vector(N);
  std::cout << v << '\n';

  // sum all the elements of the vector
  const auto sum = std::accumulate(v.begin(), v.end(), 0);
  std::cout << "The sum is " << sum << std::endl;

  // compute the average of the first half and of the second half of the vector
  const float avg_f = std::reduce(v.begin(), v.begin()+N/2);
  std::cout << "The avg of first half is " << avg_f / (N/2) << std::endl;
  const float avg_s = std::reduce(v.begin() + N/2, v.end());
  std::cout << "The avg of second half is " << avg_s / (N/2) << std::endl;

  // move the three central elements to the beginning of the vector
  std::rotate(v.begin(), v.begin() + N/2 - 1, v.begin() + N/2 + 2);
  std::cout << "rotated v:\n";
  std::cout << v << '\n';

  // remove duplicate elements
  std::ranges::sort(v);
  // we first sort because unqiue removes duplicate adjacent(!) elements
  const auto [first, last] = std::ranges::unique(v);
  v.erase(first, last);
  std::cout << "sorted and unique v:\n";
  std::cout << v << '\n';
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
  std::random_device rd;
  std::default_random_engine eng{rd()};

  int const MAX_N = 100;
  std::uniform_int_distribution<int> dist{1, MAX_N};

  std::vector<int> result;
  result.reserve(N);
  std::generate_n(std::back_inserter(result), N, [&] { return dist(eng); });

  return result;
}
