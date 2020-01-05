#include "mpi.h"
#include <stdio.h>
#include <malloc.h>
#define M 20
#define Time 1
#define dt 0.01
#define dx 0.1
#define D 0.1

void Khoitao(double *T) {
    for (int i = 1; i <= M + 1; i++) {
        T[i] = 25.0;
    }
    T[0] = 100.0;
}

int main(int argc, char **argv) {
    double *T = (double *) malloc(M * sizeof(double));
    double *dT = (double *) malloc(M * sizeof(double));

    Khoitao(T);

    MPI_Init(&argc, &argv);

    int rank, size, seg, nums;
    int Ntime = Time/dt;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stt;
    seg = M / size;
    nums = M / seg;
    double *rT = (double *) malloc(seg * sizeof(double));
    double *rdT = (double *) malloc(seg * sizeof(double));
    MPI_Request *rlrequest = (MPI_Request *) malloc(sizeof(MPI_Request));
    MPI_Request *rrrequest = (MPI_Request *) malloc(sizeof(MPI_Request));
    MPI_Request *slrequest = (MPI_Request *) malloc(sizeof(MPI_Request));
    MPI_Request *srrequest = (MPI_Request *) malloc(sizeof(MPI_Request));
    double *lb = (double *) malloc(sizeof(double));
    double *rb = (double *) malloc(sizeof(double));

    MPI_Scatter(T, seg, MPI_DOUBLE, rT, seg, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        *lb = 100.0;
    } else if (rank == size - 1) {
        *rb = 25.0;
    }

    if (rank == 0) {
        printf("%6.2f: ", 0 * dt);
        for (int s = 0; s < M; s++) {
            printf("%5.2f, ", T[s]);
        }
        printf("\n");
    }

    for (int t = 1; t < Ntime; t ++) {

        if (rank < size - 1) {
            MPI_Isend(rT + seg - 1, 1, MPI_DOUBLE, rank + 1, t, MPI_COMM_WORLD, srrequest);
            MPI_Irecv(rb, 1, MPI_DOUBLE, rank + 1, t, MPI_COMM_WORLD, rrrequest);
        }

        if (rank > 0) {
            MPI_Isend(rT, 1, MPI_DOUBLE, rank - 1, t, MPI_COMM_WORLD, slrequest);
            MPI_Irecv(lb, 1, MPI_DOUBLE, rank - 1, t, MPI_COMM_WORLD, rlrequest);
        }

        for (int s = 1; s < seg - 1; s++) {
            rdT[s] = D * (rT[s - 1] - 2 * rT[s] + rT[s + 1]) / (dx * dx);
        }
        rdT[0] = D * (-2 * rT[0] + rT[1]) / (dx * dx);
        rdT[seg - 1] = D * (rT[seg - 2] - 2 * rT[seg - 1]) / (dx * dx);

        for (int s = 1; s < seg - 1; s++) {
            rT[s] += rdT[s] * dt;
        }

        if (rank < size - 1) {
            MPI_Wait(rrrequest, &stt);
            rdT[seg - 1] += D * (*rb) / (dx * dx);
            rT[seg - 1] += rdT[seg - 1] * dt;
        }

        if (rank > 0) {
            MPI_Wait(rlrequest, &stt);
            rdT[0] += D * (*lb) / (dx * dx);
            rT[0] += rdT[0] * dt;
        }

        MPI_Gather(rT, seg, MPI_DOUBLE, T, seg, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        if (rank == 0) {
            printf("%6.2f: ", t * dt);
            for (int s = 0; s < M; s++) {
                printf("%5.2f, ", T[s]);
            }
            printf("\n");
        }
    }

    // MPI_Gather(rT, seg, MPI_DOUBLE, T, seg, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Finalize();

    // if (rank == 0) {
    //     for (int s = 0; s < M; s++) {
    //     printf("%5.2f, ", T[s]);
    //     }
    //     printf("\n");
    // }
    
}