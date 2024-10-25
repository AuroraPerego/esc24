#include <iostream>
#include <iterator>
#include <cassert>
#include <vector>

std::ostream& operator<<(std::ostream& os, std::vector<int> const& c)
{
  std::copy(
    std::begin(c),
    std::end(c),
    std::ostream_iterator<int>{os, " "}
    );

  return os;
}


constexpr std::vector<std::vector<int>> compute_pascal(const int N)
{
  assert(N>0);
  std::vector<std::vector<int>> pas;
  pas.reserve(N);
  pas[0].push_back(1);
  for (int i = 1; i < N; ++i) {
    pas[i].reserve(i+1);
    pas[i].push_back(1);
    for (int j = 1; j < i; ++j) {
      pas[i].push_back(pas[i-1][j] + pas[i-1][j-1]);
    }
    pas[i].push_back(1);
  //std::cout << pas[i] << std::endl;
  }
  return pas;
}

int main()
{
  constexpr int N = 10;
  int ii = 0;
  const auto vv = compute_pascal(N);
  for (const auto& v : vv) {
    ++ii;
    std::string space(N - ii, ' ');
    std::cout << space << v << space << std::endl;
  }
  return 0;
}
