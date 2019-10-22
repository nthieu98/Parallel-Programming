#include "mpi.h"
#include "stdio.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
    int m = 10;
    int n = 10;
    int rank, size, seg;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stt;
    seg = m / size;
    if(rank == 0) {
        
        int **A = (int **) malloc(m * sizeof(int*));
        for(int i = 0; i < m; i++){
            A[i] = (int *) malloc(n * sizeof(int));
        }
        int **B = (int **) malloc(m * sizeof(int*));
        for(int i = 0; i < m; i++){
            B[i] = (int *) malloc(n * sizeof(int));
        }
        int **C = (int **) malloc(m * sizeof(int*));
        for(int i = 0; i < m; i++){
            C[i] = (int *) malloc(n * sizeof(int));
        }
        for(int i = 0; i < m; i++) {
            for(int j = 0; j < n; j++) {
                A[i][j] = B[i][j] = 1;
            }
        }
        // printf("%d\n", **(A + seg * n));
        for(int i = 1; i < size; i++) {
            for(int j = 0; j < seg; j++) {
                MPI_Send(A[i * seg + j], n, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(B[i * seg + j], n, MPI_INT, i, 1, MPI_COMM_WORLD);
            // printf("0 sent data to %d\n", i);
            }
        }
        for(int i = 0; i < seg; i++) {
            for(int j = 0; j < n; j++){
                C[i][j] = A[i][j] + B[i][j];
            }
        }
        for(int i = 1; i < size; i++) {
            MPI_Recv(C + i * seg * n, seg * n, MPI_INT, i, 0, MPI_COMM_WORLD, &stt);
        }
        for(int i = 0; i < m; i++) {
            for(int j = 0; j < n; j++) {
                printf("%d ", C[i][j]);
                // if(i % 10 == 9) printf("\n");
            }
            printf("\n");
        }
    }
    else {
        int **Aa = (int **) malloc(seg * sizeof(int*));
        for(int i = 0; i < seg; i++){
            Aa[i] = (int *) malloc(n * sizeof(int));
        }
        int **Bb = (int **) malloc(seg * sizeof(int*));
        for(int i = 0; i < seg; i++){
            Bb[i] = (int *) malloc(n * sizeof(int));
        }
        int **Cc = (int **) malloc(seg * sizeof(int*));
        for(int i = 0; i < seg; i++){
            Cc[i] = (int *) malloc(n * sizeof(int));
        }
        MPI_Recv(Aa, seg * n, MPI_INT, 0, 0, MPI_COMM_WORLD, &stt);
        MPI_Recv(Bb, seg * n, MPI_INT, 0, 1, MPI_COMM_WORLD, &stt);
        // printf("%d recived data from 0\n", rank);
        for(int i = 0; i < seg; i++) {
            for(int j = 0; j < n; j++) {
                Cc[i][j] = Aa[i][j] + Bb[i][j];
            }
        }
        MPI_Send(Cc, seg * n, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}