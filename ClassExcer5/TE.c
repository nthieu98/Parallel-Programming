#include "mpi.h"
#include <stdio.h>
#include <malloc.h>
#define M 20
#define Time 1
#define dt 0.01
#define dx 0.1
#define D 0.1

void Khoitao(float *T)
{
    for (int i = 0; i < M; i++)
    {
        *(T+i) = 25.0;
    }
}

int main(int argc, char **argv)
{
    float *T = (float *) malloc(M*sizeof(float));
    float *dT = (float *) malloc(M*sizeof(float));
    
    Khoitao(T);
    
    MPI_Init(&argc, &argv);
    int Ntime = Time/dt;
    for (int t = 0; t < Ntime; t ++)
    {     
        // DHB2(T, dT);
       // t = t +dt;
        int rank, size, seg, nums;
        
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Status stt;
        seg = M / size;
        nums = M / seg;
        
        if(rank == 0){
            printf("t = %5.2f, T = {", t * dt);
            for (int i = 0; i < M; i++){
                printf("%5.2f, ", T[i]);
            }
            printf("\b\b}\n");
            float *lb = (float *) malloc(sizeof(float));
            float *rb = (float *) malloc(sizeof(float));
            for(int i = 1; i < nums; i++){
                *lb = T[i * seg - 1];
                *rb = (i == nums - 1) ? 25.0 : T[(i + 1) * seg];
                // printf("%0.2f, %0.2f\n", *lb, *rb);
                MPI_Send(T + i * seg, seg, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
                MPI_Send(lb, 1, MPI_FLOAT, i, 1, MPI_COMM_WORLD);
                MPI_Send(rb, 1, MPI_FLOAT, i, 2, MPI_COMM_WORLD);
            }
            float c, l, r;
            for(int i = 0; i < seg; i++) {
                c = T[i];
                l = (i == 0) ? 100 : T[i - 1];
                r = (i == M-1)? 25 : T[i + 1];
                // printf("%0.2f %.2f %.2f\n", c, l, r);
                dT[i] = D * (l - 2 * c + r) / (dx * dx);
                
            }
            for (int i = 0; i < seg; i++){
			    T[i] = T[i] + dT[i] * dt;
            }
            for(int i = 1; i < nums; i++) {
                MPI_Recv(T + i * seg, seg, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &stt);
            }
        }
        else {
            float *dT1 = (float *) malloc(seg * sizeof(float));
            float *T1 = (float *) malloc(seg * sizeof(float));
            float *llb = (float *) malloc(sizeof(float));
            float *rrb = (float *) malloc(sizeof(float));
            MPI_Recv(T1, seg, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &stt);
            MPI_Recv(llb, 1, MPI_FLOAT, 0, 1, MPI_COMM_WORLD, &stt);
            MPI_Recv(rrb, 1, MPI_FLOAT, 0, 2, MPI_COMM_WORLD, &stt);
            float c, l, r;
            // printf("rank: %d, llb: %.2f, rrb: %.2f\n", rank, *llb, *rrb);
            for(int i = 0; i < seg; i++) {
                c = T1[i];
                l = (i == 0) ? *llb : T1[i - 1];
                r = (i == seg - 1) ? *rrb : T1[i + 1];
                // printf("%d %0.2f %.2f %.2f\n",rank, c, l, r);
                dT1[i] = D * (l - 2 * c + r) / (dx * dx);
            }
            for (int i = 0; i < seg; i++){
			    T1[i] = T1[i] + dT1[i] * dt;
            }
            MPI_Send(T1, seg, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
}