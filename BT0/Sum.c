#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main()
{
  //freopen("out.txt", "w", stdout);
  int m, n;
  scanf("%d %d", &m, &n);
  int *A = (int *) malloc(m * sizeof(int));
  for(int i = 0; i < m; i++){
    A[i] = i;
  }
  int id, j, sumSeg, sum;
  int c = m/n;
  sum = 0;
  omp_set_num_threads(n);
  #pragma omp parallel private (id, j, sumSeg)
  {
    id = omp_get_thread_num();
    sumSeg = 0;
    for(int i = 0; i < c; i++){
      j = id*c+i;
      printf("%d %d\n", j, A[j]);
      sumSeg += A[j];
    }
    if(id == n-1){
      j = (id+1)*c;
      for(; j < m; j++){
        printf("%d %d\n", j, A[j]);
        sumSeg += A[j];
      }
    }
    sum += sumSeg;
  }

  printf("%d", sum);
  return 0;
}
