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
#include <chrono>
#include <mpi.h>

static long num_steps = 100000000;
double step;
int main (int argc, char** argv)
{
	  double x, pi, sum = 0.0;

	  step = 1.0/(double) num_steps;

    int size, rank, nameLen;
    char name[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(name, &nameLen);

	  auto start_time = std::chrono::steady_clock::now();

	  for (int i = rank; i<= num_steps; i+=size){
		  x = (i-0.5)*step;
		  sum += 4.0/(1.0+x*x);
	  }

    MPI_Reduce(&sum, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    // count = num of elements in buf on which we're doing the reduction

	  pi *= step ;
	  const std::chrono::duration<double> run_time = std::chrono::steady_clock::now() - start_time;
    if (rank == 0) {
      printf("pi with %ld steps is %lf in %lf seconds ",num_steps,pi,run_time);
      std::cout << "from rank " << rank+1 << " of " << size << " on node " << name << "\n";
    }

    MPI_Finalize();
}





