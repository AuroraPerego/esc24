#include <cstdlib>
#include <iostream>
#include <memory>
#include <span>

class Deleter{
  public:
    void operator()(int* ptr){
      delete[] ptr;
    }
};

void do_something_with(std::span<int> a);

int main()
{
  // allocate memory for 1000 int's
  int const SIZE = 1000;
  std::unique_ptr<int, Deleter> p(new int[SIZE]);
  do_something_with({p.get(), SIZE});
}

void do_something_with(std::span<int> a)
{
  std::fill(a.begin(), a.end(), 42);
}
