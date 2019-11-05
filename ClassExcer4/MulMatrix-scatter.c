#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int m, n, p;
    m = 20;
    n = 10;
    p = 20;
    int rank, size, seg;
    int *A = (int *) malloc(m * n * sizeof(int));
    int *B = (int *) malloc(n * p * sizeof(int));
    int *C = (int *) malloc(m * p * sizeof(int));
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stt;
    seg = m / size;
    
    if(rank == 0) { 
        for(int i = 0; i < m; i++) {
            for(int j = 0; j < n; j++) {
                A[i * n + j] = i;
            }
        }
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < p; j++) {
                B[i * p + j] = i;
            }
        }
    }
    int *Aa = (int *) malloc(seg * n * sizeof(int));
    // int *Bb = (int *) malloc(n * p * sizeof(int));
    int *Cc = (int *) malloc(seg * p * sizeof(int));
    MPI_Scatter(A, seg * n, MPI_INT, Aa, seg * n, MPI_INT, 0, MPI_COMM_WORLD); 
    MPI_Bcast(B, n * p, MPI_INT, 0, MPI_COMM_WORLD);
    // MPI_Scatter(B, seg * n, MPI_INT, Bb, seg * n, MPI_INT, 0, MPI_COMM_WORLD);
    // printf("%d\n", rank);
    for(int i = 0; i < seg; i++) {
        for(int j = 0; j < p; j++) {
            Cc[i * p + j] = 0;
            for(int k = 0; k < n; k++){
                Cc[i * p + j] += Aa[i * n + k] * B[k * p + j];
            }
            // printf("%d ", Aa[i * n + j]);
        }
    }
    MPI_Gather(Cc, seg * p, MPI_INT, C, seg * p, MPI_INT, 0, MPI_COMM_WORLD);
    if(rank == 0) {
        for(int i = 0; i < m; i++) {
            for(int j = 0; j < p; j++) {
                printf("%d ", C[i * p + j]);
            // Cc[i * n + j] = Aa[i * n + j] + Bb[i * n + j];
            }
            printf("\n");
        }
    }
    // printf("\n");
    MPI_Finalize();
}