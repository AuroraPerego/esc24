#include "oneapi/tbb.h"
#include "tbb/tick_count.h"
#include <iostream>
#include <mutex>

int main() {
    const int num_steps = 1000000;
    const double step = 1.0 / num_steps;
    double sum = 0.0;
    auto start_time = tbb::tick_count::now();

    // Mutex to protect shared access to the sum
    std::mutex mutex;

    tbb::parallel_for(0, num_steps, 1, [&](int i) {
        double x = (i - 0.5) * step;
        double partial_sum = 4.0 / (1.0 + x * x);

        // Protect access to the shared variable using a mutex
        mutex.lock();
        sum += partial_sum;
        mutex.unlock();
    });

    double pi = sum * step;
	  auto run_time = tbb::tick_count::now() - start_time;
	  printf("\n pi with %ld steps is %lf in %lf seconds\n ", num_steps, pi, run_time.seconds());

    return 0;
}
