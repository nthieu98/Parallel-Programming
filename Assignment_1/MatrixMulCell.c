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
  int segRow = m / numThr + ((m % numThr) ? 1 : 0);
  int segCol = p / numThr + ((p % numThr) ? 1 : 0);
  printf("segRow: %d\n", segRow);
  printf("segCol: %d\n", segCol);
  int numSegRow = ceil(1.0 * m / segRow);
  int numSegCol = ceil(1.0 * p / segCol);
  printf("numSegRow: %d\n", numSegRow);
  printf("numSegCol: %d\n", numSegCol);
  int numCell = numSegRow * numSegCol;
  printf("Num Cell: %d\n", numCell);
  int id;
  omp_set_num_threads(numThr);
  #pragma omp parallel private(id)
  {
    id = omp_get_thread_num();
    //printf("%d", id);
    for(int cell = id; cell < numCell; cell += numThr) {
      int row = cell / numSegCol;
      int col = cell - row * numSegCol;
      //printf("row col: %d %d\n", row * segRow, col * segCol);
      for(int i = row * segRow; i < (row + 1) * segRow && i < m; i ++) {
        for(int j = col * segCol; j < (col + 1) * segCol && j < p; j ++) {
          for(int k = 0; k < n; k ++) {
            C[i][j] = C[i][j] + A[i][k] * B[k][j];
          }
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

