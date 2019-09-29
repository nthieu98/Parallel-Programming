#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

int main()
{
  int m;
  scanf("%d", &m);
  int *A = malloc(m * sizeof(int));
  for(int i = 0; i < m; i++){
    scanf("%d", &A[i]);
  }
  //int k = ceil(log2(m));
  //printf("%d", k);
  int step = 1;
  //int l = m;
  while(m > 1){
    int numThr = m/2;
    //printf("numThr %d\n", numThr);
    omp_set_num_threads(numThr);
    int id, s;
    #pragma omp parallel private (id, s)
    {
      id = omp_get_thread_num();
      //printf("%d %d %d\n", id, id * 2 * step, (id * 2 + 1) * step);
      A[id * 2 * step] = A[id * 2 * step] + A[(id * 2 + 1) * step];
      if(id == numThr - 1 && m % 2 == 1) A[id * 2 * step] += A[(id * 2 + 2) * step];
    }
    m /= 2;
    step *= 2;
    /**for(int i = 0; i < l; i++) {
      printf("%d ", A[i]);
    }
    printf("\n");**/
  }
  printf("%d", A[0]);
  return 0;
}
