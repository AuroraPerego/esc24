#include <iostream>
#include <random>

typedef unsigned long int Type;

class LinearCongruential
{
  public:
  // constructor
  LinearCongruential() :
    seed_(1) {};

  LinearCongruential(Type seed) :
    seed_(seed) {};

  auto operator()()
  {
    // returns x_{n+1} = 16807 * x_n mod (2^31 - 1)
    seed_ = (16807ul * seed_) % (1ul<<31 - 1);
    return seed_;
  };
  private:
    Type seed_;
};

int main()
{
  LinearCongruential eng;
  std::default_random_engine stdeng;
  std::cout << eng() << '\t' << stdeng() << '\n';
  std::cout << eng() << '\t' << stdeng() << '\n';
  std::cout << eng() << '\t' << stdeng() << '\n';
  std::cout << eng() << '\t' << stdeng() << '\n';
}
