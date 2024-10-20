#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <thread>

#include "tbb/tick_count.h"
#include "tbb/tbb.h"
#include <oneapi/tbb/mutex.h>

int main(int argc, char **argv)
{
    const int num_steps = 100000000;
    const double step = 1.0 / num_steps;
    auto values = std::vector<double>(num_steps);

    std::size_t NTHREADS = 1;
    const char* threads_env = std::getenv("NTHREADS");
    if (threads_env != nullptr and std::strlen(threads_env) != 0) {
      NTHREADS = std::atoi(threads_env);
    }

    tbb::global_control tbb_max_threads{tbb::global_control::max_allowed_parallelism,
                                          static_cast<std::size_t>(NTHREADS)};
                                          std::cout << NTHREADS << "\n";

    double pi = 0.;
    tbb::spin_mutex m;
    const auto start_time = tbb::tick_count::now();
    tbb::parallel_for( tbb::blocked_range<int>(0,num_steps),
                       [&](tbb::blocked_range<int> r)
    {
        double local_sum = 0.;
        for (int i=r.begin(); i<r.end(); ++i)
        {
            const double x = (i - 0.5) * step;
            local_sum +=   4.0*step / (1.0 + x * x);
        }
        tbb::spin_mutex::scoped_lock l(m);
        pi += local_sum;
    });

	  const auto run_time = tbb::tick_count::now() - start_time;
	  printf("pi with %ld steps is %lf in %lf seconds\n ", num_steps, pi, run_time.seconds());

    return 0;
}
