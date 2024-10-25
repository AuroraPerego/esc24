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
#include <mpi.h>

int main (int argc, char** argv)
{

    int size, rank, nameLen;
    char name[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(name, &nameLen);
    MPI_Barrier(MPI_COMM_WORLD);
    double init_time;
    if(rank ==0)
      init_time = MPI_Wtime();

    // MPI_Status MyStat;
    //int count;
    float message = 1.f;
    if(rank ==1)
      int rerr = MPI_Recv(&message, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if(rank ==0)
      int serr = MPI_Send(&message, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);

    if (rank == 0) {
      double run_time = MPI_Wtime()-init_time;
      printf("message sent/received in %lf microseconds\n", run_time*1e6);
    }

    MPI_Finalize();
}





