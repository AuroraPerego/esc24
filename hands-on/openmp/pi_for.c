/*

This program will numerically compute the integral of

                  4/(1+x*x)

from 0 to 1.  The value of this integral is pi -- which
is great since it gives us an easy way to check the answer.

The is the original sequential program.  It uses the timer
from the OpenMP runtime library

History: Written by Tim Mattson, 11/99.

*/
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <omp.h>
static long num_steps = 100000000;
double step;
int main ()
{
	  double pi, sum = 0.0;
	  double start_time, run_time;

	  step = 1.0/(double) num_steps;

    int NTHREADS = 1;
    const char* threads_env = std::getenv("NTHREADS");
    if (threads_env != nullptr and std::strlen(threads_env) != 0) {
      NTHREADS = std::atoi(threads_env);
    }
    omp_set_num_threads(NTHREADS);
//    std::cout << " running with " << NTHREADS << " threads\n";

	  start_time = omp_get_wtime();

#pragma omp parallel for reduction(+:sum)
	  for (int i=1;i<= num_steps; i++){
		  double x = (i-0.5)*step;
		  sum = sum + 4.0/(1.0+x*x);
	  }

	  pi = step * sum;
	  run_time = omp_get_wtime() - start_time;
	  printf("pi with %ld steps is %lf in %lf seconds\n ",num_steps,pi,run_time);
}





