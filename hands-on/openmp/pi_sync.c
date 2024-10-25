/*

This program will numerically compute the integral of

                  4/(1+x*x)

from 0 to 1.  The value of this integral is pi -- which
is great since it gives us an easy way to check the answer.

The is the original sequential program.  It uses the timer
from the OpenMP runtime library

History: Written by Tim Mattson, 11/99.

*/
#include <stdio.h>
#include <omp.h>
//static long num_steps = 100; //100000000;

#define NTHREADS 10

static long num_steps = 100000000;
double step;
int main ()
{
	  // int i;
	  double x, pi, sum = 0.0;
	  double start_time, run_time;

	  step = 1.0/(double) num_steps;

    omp_set_num_threads(NTHREADS);

	  start_time = omp_get_wtime();

#pragma omp parallel
{
    int nthreads = omp_get_num_threads();
    double partial_sum = 0;
    double partial_x = 0;
	  for (int i = omp_get_thread_num(); i <= num_steps; i += nthreads){
		  partial_x = (i-0.5)*step;
		  partial_sum +=  4.0/(1.0+partial_x*partial_x);
	  }

#pragma omp critical
	  pi += step * partial_sum;
}

    run_time = omp_get_wtime() - start_time;
	  printf("\n pi with %ld steps is %lf in %lf seconds\n ",num_steps,pi,run_time);
}





