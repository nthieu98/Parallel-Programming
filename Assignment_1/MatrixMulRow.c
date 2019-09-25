#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int main()
{
  int m, n, p, numThr;
  scanf("%d", &numThr);
  scanf("%d %d %d", &m, &n, &p);
  int **A = (int **) malloc(m * sizeof(int*));
  for(int i = 0; i < m; i++){
    A[i] = (int *) malloc(n * sizeof(int));
  }
  int **B = (int **) malloc(n * sizeof(int*));
  for(int i = 0; i < n; i++){
    B[i] = (int *) malloc(p * sizeof(int));
  }
  int **C = (int **) malloc(m * sizeof(int*));
  for(int i = 0; i < m; i++){
    C[i] = (int *) malloc(p * sizeof(int));
  }
  for(int i = 0; i < m; i++){
    for(int j = 0; j < n; j++){
      A[i][j] = 1;
    }
  }
  for(int i = 0; i < n; i++){
    for(int j = 0; j < p; j++){
      B[i][j] = 1;
    }
  }
  int numSeg = m / numThr + ((m % numThr) ? 1 : 0);
  int id;
  omp_set_num_threads(numThr);
  #pragma omp parallel private(id)
  {
    id = omp_get_thread_num();
    //printf("%d", id);
    for(int i = id * numSeg; i < m && i < (id + 1) * numSeg; i++){
      for(int j = 0; j < p; j++){
        for(int k = 0; k < n; k++){
          C[i][j] += A[i][k] * B[k][j];
        }
      }
    }
  }
  for(int i = 0; i < m; i++){
    for(int j = 0; j < p; j++) {
      printf("%d ", C[i][j]);
    }
    printf("\n");
  }
  return 0;
}
