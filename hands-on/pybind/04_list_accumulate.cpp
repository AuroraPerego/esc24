#include <iostream>
#include <numeric>
#include <pybind11/embed.h>

namespace py = pybind11;

// Step 2: Write a function that takes a Python list of integers and returns the sum
// of its elements

int main () {
  // Step 1: Create an interpreter
  py::scoped_interpreter guard{};
  py::list li;

  for (int i = 0; i <= 10; ++i)
    li.append(i);

  // Step 3: Create a Python list and fill it with integers from 0 to 10
  // Hint: Use the ranges library and C++20 views

  // Step 4: Compute the sum of the list's elements and print the result
  auto sum = std::accumulate(li.begin(), li.end(), 0);
  std::cout << sum << '\n';

}
