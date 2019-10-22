#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int m, n;
    m = 10;
    n = 10;
    int rank, size, seg;
    int *A = (int *) malloc(n * m * sizeof(int));
    int *B = (int *) malloc(n * m * sizeof(int));
    int *C = (int *) malloc(n * ms * sizeof(int));
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    seg = m / size;
    MPI_Status stt;
    if(rank == 0) { 
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < m; j++) {
                A[i * n + j] = i * n + j;
                B[i * n + j] = i * n + j;
            }
        }
    }
    int *Aa = (int *) malloc(seg * m * sizeof(int));
    int *Bb = (int *) malloc(n * m * sizeof(int));
    int *Cc = (int *) malloc(seg * m * sizeof(int));
    MPI_Scatter(A, seg * m, MPI_INT, Aa, seg * m, MPI_INT, 0, MPI_COMM_WORLD); 
    MPI_Bcast(B, n * m, MPI_INT, 0, MPI_COMM_WORLD);
    // MPI_Scatter(B, seg * n, MPI_INT, Bb, seg * n, MPI_INT, 0, MPI_COMM_WORLD);
    // printf("%d\n", rank);
    for(int i = 0; i < seg; i++) {
        for(int j = 0; j < n; j++) {
            // printf("%d ", Aa[i * n + j]);
            Cc[i * n + j] = Aa[i * n + j] + Bb[i * n + j];
        }
    }
    MPI_Gather(Cc, seg * n, MPI_INT, C, seg * n, MPI_INT, 0, MPI_COMM_WORLD);
    if(rank == 0) {
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < m; j++) {
                printf("%d ", C[i * n + j]);
            // Cc[i * n + j] = Aa[i * n + j] + Bb[i * n + j];
            }
            printf("\n");
        }
    }
    // printf("\n");
    MPI_Finalize();
}