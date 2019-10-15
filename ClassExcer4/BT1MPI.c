#include "mpi.h"
#include "stdio.h"

int main(int argc, char **argv)
{
    int rank, size, seg;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stt;
    seg = 100 / size;
    int A[100], B[100], C[100];
    if(rank == 0) {
        for(int i = 0; i < 100; i++) A[i] = B[i] = i;
        for(int i = 1; i < size; i++) {    
            MPI_Send(A + i * seg, seg, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(B + i * seg, seg, MPI_INT, i, 1, MPI_COMM_WORLD);
            // printf("0 sent data to %d\n", i);
        }
        for(int i = 0; i < seg; i++) C[i] = A[i] + B[i];
    }
    else {
        int Atmp[seg], Btmp[seg], Ctmp[seg];
        MPI_Recv(Atmp, seg, MPI_INT, 0, 0, MPI_COMM_WORLD, &stt);
        MPI_Recv(Btmp, seg, MPI_INT, 0, 1, MPI_COMM_WORLD, &stt);
        // printf("%d recived data from 0\n", rank);
        for(int i = 0; i < seg; i++) {
            Ctmp[i] = Atmp[i] + Btmp[i];
        }
        MPI_Send(Ctmp, seg, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    if(rank == 0) {
        for(int i = 1; i < size; i++) {
            MPI_Recv(C + i * seg, seg, MPI_INT, i, 0, MPI_COMM_WORLD, &stt);
        }
        for(int i = 0; i < 100; i++) {
            printf("%d ", C[i]);
            if(i % 10 == 9) printf("\n");
        }
        
    }
    MPI_Finalize();
}