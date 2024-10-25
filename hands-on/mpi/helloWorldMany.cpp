#include <iostream>
#include <mpi.h>

int main(int argc, char** argv)
{
  int size, rank, nameLen;
  char name[MPI_MAX_PROCESSOR_NAME];
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Get_processor_name(name, &nameLen);
  std::cout << "Hello World from rank " << rank+1 << " of " << size << " on node " << name << "\n";
  MPI_Finalize();
  return 0;
}
