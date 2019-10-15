#include "mpi.h"
#include "stdio.h"

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stt;
    if(rank == 0) {
        int A[100], B[100];
        for(int i = 1; i <= 100; i++) A[i] = B[i] = i;
        
    }
}