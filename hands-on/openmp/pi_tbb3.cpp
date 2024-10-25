#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <thread>

#include "tbb/tick_count.h"
#include "tbb/tbb.h"

int main(int argc, char **argv)
{
    const int num_steps = 100000000;
    const double step = 1.0 / num_steps;

    std::size_t NTHREADS = 1;
    const char* threads_env = std::getenv("NTHREADS");
    if (threads_env != nullptr and std::strlen(threads_env) != 0) {
      NTHREADS = std::atoi(threads_env);
    }

    tbb::global_control tbb_max_threads{tbb::global_control::max_allowed_parallelism,
                                          static_cast<std::size_t>(NTHREADS)};
                                          std::cout << NTHREADS << "\n";

    const auto start_time = tbb::tick_count::now();
    auto values = std::vector<double>(num_steps);
    tbb::parallel_for( tbb::blocked_range<int>(0,num_steps),
                       [&](tbb::blocked_range<int> r)
    {
        for (int i=r.begin(); i<r.end(); ++i)
        {
            const double x = (i - 0.5) * step;
            values[i] +=   4.0*step / (1.0 + x * x);
        }
    });

    const auto pi = tbb::parallel_reduce(
                    tbb::blocked_range<int>(0,values.size()),
                    0.0,
                    [&](tbb::blocked_range<int> r, double total)
                    {
                        for (int i=r.begin(); i<r.end(); ++i)
                        {
                            total += values[i] * step;
                        }

                        return total;
                    }, std::plus<double>() );

	  const auto run_time = tbb::tick_count::now() - start_time;
	  printf("pi with %ld steps is %lf in %lf seconds\n ", num_steps, pi, run_time.seconds());

    return 0;
}
