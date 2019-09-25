#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main()
{
  //freopen("out.txt", "w", stdout);
  int m, n;
  scanf("%d %d", &m, &n);
  int *A = (int *) malloc(m * sizeof(int));
  int *B = (int *) malloc(m * sizeof(int));
  int *C = (int *) malloc(m * sizeof(int));
  for(int i = 0; i < m; i++){
    A[i] = 1;
  }
  for(int i = 0; i < m; i++){
    B[i] = 1;
  }
  int id, j;
  int c = m/n;
  omp_set_num_threads(n);
  #pragma omp parallel private (id, j)
  {
    id = omp_get_thread_num();
    for(int i = 0; i < c; i++){
      j = id*c+i;
      printf("thread %d %d \n",id, j);
      C[j] = A[j]+B[j];
    }
    if(id == n-1){
      j = (id+1)*c;
      for(; j < m; j++){
        printf("thread %d %d \n",id, j);
        C[j] = A[j] + B[j];
      }
    }
  }
  for(int i = 0; i < m; i++){
    printf("%d ", C[i]);
  }
  return 0;
}
