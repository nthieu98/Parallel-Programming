#include <omp.h>
#include <stdio.h>

int main(){
  int id, x;
  omp_set_num_threads(100);
  #pragma omp parallel private(id, x)
  {
    //printf("%d", omp_get_thread_limit());
    id = omp_get_thread_num();
    x = 10*id;
    printf("\n");
    printf("Hello from thread %d, x = %d", id, x);
    printf("\n");
    //printf("Hello from thread %d, nthreads %d\n", omp_get_thread_num(), omp_get_num_threads());
  }
	return 0;
}
