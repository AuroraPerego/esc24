#include <omp.h>
#include <stdio.h>
int main ()
{
  omp_set_num_threads(40);
  #pragma omp parallel
  //printf("num threads: %d\n",  omp_get_num_threads());
  {
    printf("tid : %d\n", omp_get_thread_num());
    printf("Hello ");
    printf("World \n");
  }
}
