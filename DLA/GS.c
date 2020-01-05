
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "mpi.h"

const long N = 100000 + 7;
const long INF = 1000000000 + 7;
const long MODULE = 1000000000 + 7;
// const double ne = 2.0;

#define n 128
#define epsilon 0.001
#define tolerance 0.001
#define eta 2.0
#define omega 1.9
#define numIter 1000

int dx[4] = {-1, 0, 0, 1}, dy[4] = {0, -1, 1, 0};

int NP, rank, mc, i, j, size, r;
float *C;
float delta;
float *Cn, *Cs, *Cl, *Cr;
int *Object, *Obj, *candidates;
float *nutris, *nutri;
MPI_Status status;


float max(float a, float b)
{
  return (a - b > 0.00000001) ? a : b;
}

float r2()
{
  return (float)rand() / (float)RAND_MAX;
}

void copy_array(float *C, float *Cn, int M, int N)
{
  for (int i = 0; i < M + 1; i++)
    for (int j = 0; j < N; j++)
      *(C + i * n + j) = *(Cn + i * n + j);
}

void print_array_float(float* Arr)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++)
			printf("%0.4f\t", *(Arr+i*n+j));
		printf("\n");
	}
}

void print_array_int(int* Arr)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++)
			if(*(Arr + i * n + j) == 1) printf("%d\t", *(Arr+i*n+j));
      else printf("-\t");
		printf("\n");
	}
}

void iteratorLog(int iter, float *C, int *Obj) {
	char *logFile = (char *) malloc (1024 * sizeof(char));
	sprintf(logFile, "./log/log_N=%d_eta=%0.2lf_iter=%d.log", n, eta, iter);
	FILE *f = fopen(logFile, "w");
	int l, m;
	for (l = 0; l < n; l++) {
		for (m = 0; m < n; m++) {
			fprintf(f, "%lf ", C[n * l + m]);
		}
		fprintf(f, "\n");
	}
	fprintf(f, "\n");
	for (l = 0; l < n; l++) {
		for (m = 0; m < n; m++) {
			fprintf(f, "%d ", Obj[n * l + m]);
		}
		fprintf(f, "\n");
	}
	printf("%s\n", logFile);
	free(logFile);
}

void Init()
{
  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {
      if (i == 0)
        *(C + i * n + j) = 1.0;
    
      else
        *(C + i * n + j) = 0.0;

      *(Object + i * n + j) = 0;
    }
  }
  *(Object + (n - 1) * n + (n / 2)) = 1;  
}

void calc_red_back(float *C, float *Cn, int M, int N, int rb, int rank, float *delta)
{
  for (int i = 1; i <= M; i++)
    for (int j = 0; j < N; j++)
    {
      int ii = M * rank + i - 1;
      int ij = j;
      if (*(Obj + i * N + j) == 1){
        *(Cn + i * N + j) = *(C + i * N + j);
        continue;
      }
      if (rank == 0 && i == 1){
        *(Cn + i * N + j) = *(C + i * N + j);
        continue;
      }
      if (rank == NP - 1 && i == M){
        *(Cn + i * N + j) = *(C + i * N + j);
        continue;
      }
      if ((ii + ij) % 2 == rb)
      {
        *(Cn + i * N + j) = (omega / 4) * (*(C + (i - 1) * N + j) + *(C + (i + 1) * N + j) 
                            + *(C + i * N + ((j - 1 + N) % N)) + *(C + i * N + ((j + 1) % N))) 
                            + (1 - omega) * (*(C + i * N + j));
        // if(rank == 0) {
        //   printf("debug %d %d %d %d %d %0.2f %0.2f %0.2f %0.2f %0.2f\n ", 
        //   ii, ij, rb, i, j, 
        //   *(C + (i - 1) * N + j), *(C + (i + 1) * N + j), 
        //   *(C + i * N + ((j - 1 + N) % N)), *(C + i * N + ((j + 1) % N)), 
        //   *(Cn + i * N + j));
        // }
        if (fabs(*(Cn + i * N + j) - *(C + i * N + j)) > *delta)
          // printf("abs %0.2f\n", fabs(*(Cn + i * N + j) - *(C + i * N + j)));
          *delta = fabs(*(Cn + i * N + j) - *(C + i * N + j));
      }
      else
      {
        *(Cn + i * N + j) = *(C + i * N + j);
      }
    }
    // printf("%0.2f\n", *delta);
}

void diffusion()
{
  do
  {
    delta = 0.0;
    //Calc red node
    if (rank != NP - 1)
    {
      MPI_Send(Cs + mc * n, n, MPI_FLOAT, rank + 1, rank, MPI_COMM_WORLD);
    }
    if (rank != 0)
    {
      MPI_Recv(Cs, n, MPI_FLOAT, rank - 1, rank - 1, MPI_COMM_WORLD, &status);
    }

    if (rank != 0)
    {
      MPI_Send(Cs + n, n, MPI_FLOAT, rank - 1, rank, MPI_COMM_WORLD);
    }
    if (rank != NP - 1)
    {
      MPI_Recv(Cs + (mc + 1) * n, n, MPI_FLOAT, rank + 1, rank + 1, MPI_COMM_WORLD, &status);
    }
    
    calc_red_back(Cs, Cn, mc, n, 0, rank, &delta);
    copy_array(Cs, Cn, mc, n);
    //Calc black node
    if (rank != NP - 1)
    {
      MPI_Send(Cs + mc * n, n, MPI_FLOAT, rank + 1, rank, MPI_COMM_WORLD);
    }
    if (rank != 0)
    {
      MPI_Recv(Cs, n, MPI_FLOAT, rank - 1, rank - 1, MPI_COMM_WORLD, &status);
    }

    if (rank != 0)
    {
      MPI_Send(Cs + n, n, MPI_FLOAT, rank - 1, rank, MPI_COMM_WORLD);
    }
    if (rank != NP - 1)
    {
      MPI_Recv(Cs + (mc + 1) * n, n, MPI_FLOAT, rank + 1, rank + 1, MPI_COMM_WORLD, &status);
    }
    calc_red_back(Cs, Cn, mc, n, 1, rank, &delta);
    copy_array(Cs, Cn, mc, n);

    //get delta
    float new_delta = 0;
    MPI_Reduce(&delta, &new_delta, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
      delta = new_delta;
      // printf("%0.5f\n", delta);
    }
    MPI_Bcast(&delta, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
  } while (delta > tolerance);
}

int main(int argc, char *argv[])
{
  // freopen("in.txt", "r", stdin);
  // freopen("out.txt", "w", stdout);
  srand(time(0));

  C = (float *)malloc(n * n * sizeof(float));
  Object = (int *) malloc(n * n * sizeof(int));
  Init();
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &NP);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  mc = n / NP;
  Cn = (float *)malloc((mc + 2) * n * sizeof(float));
  Cs = (float *)malloc((mc + 2) * n * sizeof(float));

  Obj = (int *)malloc((mc + 2) * n * sizeof(int));
  candidates = (int *)malloc((mc + 2) * n * sizeof(int));

  nutris = (float *)malloc(NP * sizeof(float));
  nutri = (float *)malloc(sizeof(float));
  
 
  MPI_Scatter(C, mc * n, MPI_FLOAT, Cs + n, mc * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Scatter(Object, mc * n, MPI_INT, Obj + n, mc * n, MPI_INT, 0, MPI_COMM_WORLD);
  if(rank == 0){
    // MPI_Gather(Cs + n, mc * n, MPI_FLOAT, C, mc * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
    // MPI_Gather(Obj + n, mc * n, MPI_INT, Object, mc * n, MPI_INT, 0, MPI_COMM_WORLD);
    iteratorLog(0, C, Object);
  }

  for (int k = 1; k <= numIter; k++){
    if(rank == 0) printf("iter : %d\n",k);
    diffusion();
    
    if (rank != NP - 1){
      MPI_Send(Obj + mc * n, n, MPI_INT, rank + 1, rank, MPI_COMM_WORLD);
    }
    if (rank != 0){
      MPI_Recv(Obj, n, MPI_INT, rank - 1, rank - 1, MPI_COMM_WORLD, &status);
    } else { 
      for(i = 0; i < n; i++) {
        *(Obj + i) = 0;
      }
    }

    if (rank != 0){
      MPI_Send(Obj + n, n, MPI_INT, rank - 1, rank, MPI_COMM_WORLD);
    }
    if (rank != NP - 1){
      MPI_Recv(Obj + (mc + 1) * n, n, MPI_INT, rank + 1, rank + 1, MPI_COMM_WORLD, &status);
    } else {
      for(i = 0; i < n; i++) {
        *(Obj + (mc + 1) * n + i) = 0;
      }
    }

    // if(rank == 0) printf("growing\n");
    for (i = 1; i <= mc; ++i)
      for (j = 0; j < n; ++j)
        *(candidates + i * n + j) = 0;

    *nutri = 0;

    for (i = 1; i <= mc; ++i) {
      for (j = 0; j < n; ++j)
      {
        if (*(Obj + i * n + j) == 1)
          continue;

        int sum = 0;
        for (r = 0; r < 4; ++r)
        {
          int u, v;
          u = i + dx[r];
          v = j + dy[r];
          if (u >= 0 && u <= mc + 1 && v >= 0 && v < n && *(Obj + u * n + v) == 1)
            sum += 1;
        }
        
        if (sum > 0)
        {
          *nutri += pow(*(Cs + i * n + j), eta);
          *(candidates + i * n + j) = 1;
        }
      }
    }

    MPI_Allgather(nutri, 1, MPI_FLOAT, nutris, 1, MPI_FLOAT, MPI_COMM_WORLD);

    float total_nutri = 0.0;

    for (i = 0; i < NP; ++i)
      total_nutri += *(nutris + i);

    for (i = 1; i <= mc; ++i) {
      for (j = 0; j < n; ++j) {
        if (*(candidates + i * n + j) == 1 && r2() <= (pow(*(Cs + i * n + j), eta) / total_nutri)){
          *(Obj + i * n + j) = 1;
          *(Cs + i * n + j) = 0.0;
          
        }
        // printf("%0.2f ", *(Cs + i*n+j));
      }
      // printf("\n");
    }
    if(k % 5 == 0){
      MPI_Gather(Cs + n, mc * n, MPI_FLOAT, C, mc * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
      MPI_Gather(Obj + n, mc * n, MPI_INT, Object, mc * n, MPI_INT, 0, MPI_COMM_WORLD);
    }
    if(rank == 0 && k % 5 == 0){
      iteratorLog(k, C, Object);
    }
    //  if(rank == 0) print_array_int(Object);
    // if(rank == 0) print_array_float(C);
  }
  diffusion();

  MPI_Gather(Cs + n, mc * n, MPI_FLOAT, C, mc * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Gather(Obj + n, mc * n, MPI_INT, Object, mc * n, MPI_INT, 0, MPI_COMM_WORLD);
  if(rank == 0) {
    printf("gathering\n");
    print_array_float(C);
    print_array_int(Object);
    // iteratorLog(numIter, C, Object);
  }
  MPI_Finalize();

  return 0;
}
