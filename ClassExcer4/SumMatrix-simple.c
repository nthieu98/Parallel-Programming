#include "mpi.h"
#include "stdio.h"
#include <stdlib.h>
#define m 10
#define n 10

int main(int argc, char **argv)
{
    int rank, size, seg;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stt;
    seg = m / size;
    if(rank == 0) {
        int *A = (int *) malloc((m * n) * sizeof(int));
        int *B = (int *) malloc((m * n) * sizeof(int));
        int *C = (int *) malloc((m * n) * sizeof(int));
        for(int i = 0; i < m; i++) {
            for(int j = 0; j < n; j++) {
                A[i * n + j] = B[i *  n + j] = i;
            }
        }
        // for(int i = 0; i < m * n; i++) printf("%d\n", A[i]);
        for(int i = 1; i < size; i++) {    
            MPI_Send(A + i * seg * n, seg * n, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(B + i * seg * n, seg * n, MPI_INT, i, 1, MPI_COMM_WORLD);
            // printf("0 sent data to %d\n", i);
        }
        for(int i = 0; i < seg; i++) 
            for(int j = 0; j < n; j++)
                C[i * n + j] = A[i * n + j] + B[i * n + j];
        for(int i = 1; i < size; i++) {
            MPI_Recv(C + i * seg * n, seg * n, MPI_INT, i, 0, MPI_COMM_WORLD, &stt);
        }
        for(int i = 0; i < m; i++) {
            for(int j = 0; j < n; j++) {
                printf("%d ", C[i * n + j]);
                // if(i % 10 == 9) printf("\n");
            }
            printf("\n");
        }
    }
    else {
        int *Aa = (int *) malloc((seg * n) * sizeof(int));
        int *Bb = (int *) malloc((seg * n) * sizeof(int));
        int *Cc = (int *) malloc((seg * n) * sizeof(int));
        MPI_Recv(Aa, seg * n, MPI_INT, 0, 0, MPI_COMM_WORLD, &stt);
        MPI_Recv(Bb, seg * n, MPI_INT, 0, 1, MPI_COMM_WORLD, &stt);
        // printf("%d recived data from 0\n", rank);
        for(int i = 0; i < seg; i++) {
            for(int j = 0; j < n; j++){
                Cc[i * n + j] = Aa[i * n + j] + Bb[i * n + j];
                // printf("%d ", Cc[i*j]);
            }
            // printf("\n");
        }
        MPI_Send(Cc, seg * n, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}