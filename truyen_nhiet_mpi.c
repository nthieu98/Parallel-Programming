#include <stdio.h>
#include <omp.h>
#include <mpi.h>

#define M 15
#define Time 1
#define dt 0.01
#define dx 0.1
#define D 0.1

int main(int argc, char *argv[])
{
	int i, j, rank, size, Mc;
	float t, *T, *prodT, *proT;
    float *finalT;

    MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;

    Mc = M / size;

    T = (float *) malloc(M * sizeof(int));
    finalT = (float *) malloc(M * sizeof(int));
    prodT = (float *) malloc(Mc * sizeof(int));
    proT = (float *) malloc(Mc * sizeof(int));

    printf("%d\n", Mc);

    if (rank == 0) {
        for (i = 0; i < M; i++)
            *(T + i) = 25;
    }

    MPI_Scatter(T, Mc, MPI_FLOAT, proT, Mc, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // send begin, send end, receive begin, receive end
    for (i = 0; i < Mc; i++)
        *(prodT + i) = 0;

    float *left, *right;

    left = (float *) malloc(sizeof(int));
    right = (float *) malloc(sizeof(int));

    if (rank == 0) {
        *left = 100.0;
        *right = 25.0;
    }
    else {
        *left = 25.0;
        *right = 25.0;
    }

    float *c, *l, *r;

    c = (float *) malloc(sizeof(int));
    l = (float *) malloc(sizeof(int));
    r = (float *) malloc(sizeof(int));

	for (t = dt; t <= Time; t = t + dt)
	{
        for (j = 0; j < Mc; j++)
        {
            *c = *(proT + j);
            *l = (j == 0) ? *left : *(proT + j - 1);
            *r = (j == Mc - 1) ? *right : *(proT + j + 1);
            *(prodT + j) = D * (*l - 2 * *c + *r) / (dx * dx);
        }
        
        printf("\n");
		printf("rank = %d, t = %5.2f, T = {", rank, t);
		for (i = 0; i < Mc; i++)
		{
			*(proT + i) = *(proT + i) + *(prodT + i) * dt;
			printf("%5.2f, ", *(proT + i));
		}
		printf("\b\b}\n");

        // send the first to left
        if (rank != 0) 
            MPI_Send(proT, 1, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD); 

        // receive from right
        if (rank != size - 1)
            MPI_Recv(right, 1, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD, &status);
        else 
            *right = 25.0;

        // send the last element to the right
        if (rank != size - 1)
            MPI_Send(proT + Mc - 1, 1, MPI_FLOAT, rank + 1, 1, MPI_COMM_WORLD);

        // receive from the left
        if (rank != 0)
            MPI_Recv(left, 1, MPI_FLOAT, rank - 1, 1, MPI_COMM_WORLD, &status);
        else
            *left = 100.0;
	}

    MPI_Gather(proT, Mc, MPI_FLOAT, finalT, Mc, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
    if(rank == 0) {
        printf("final T = {"); 
        for (i = 0; i < M; i++)
            printf("%5.2f, ", *(finalT + i));
        printf("\b\b}\n");
    }

    MPI_Finalize();
	return 0;
}