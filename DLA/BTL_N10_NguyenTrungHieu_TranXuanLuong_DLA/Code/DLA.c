
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "mpi.h"

const long N = 100000 + 7;
const long INF = 1000000000 + 7;
const long MODULE = 1000000000 + 7;

//Mảng chỉ 4 hướng xung quanh 1 ô
int dx[4] = {-1, 0, 0, 1}, dy[4] = {0, -1, 1, 0};

int n, numIter;
float eta, omega, tolerance;
int NP, rank, mc, i, j, size, r, dir;
float *C;
float delta;
float *Cnew, *Cstore, *Cl, *Cr;
int *Object, *Obj, *candidates;
float *nutriEachProc, *nutri;
MPI_Status status;

//Tìm max của 2 số thực
float max(float a, float b)
{
  return (a - b > 0.00000001) ? a : b;
}

//Tìm số thực random trong khoảng (0, 1)
float randProb()
{
  return (float)rand() / (float)RAND_MAX;
}

//Copy Csource vào Cdes
void copyArray(float *Cdes, float *Csource, int M, int N)
{
  for (int i = 0; i < M + 1; i++)
    for (int j = 0; j < N; j++)
      *(Cdes + i * n + j) = *(Csource + i * n + j);
}

//In ra màn hình mảng số thực
void printArrayFloat(float* Arr)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++)
			printf("%0.4f\t", *(Arr+i*n+j));
		printf("\n");
	}
}

// In ra màn hình mảng số nguyên
void printArrayInt(int* Arr)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++)
			if(*(Arr + i * n + j) == 1) printf("%d\t", *(Arr+i*n+j));
      else printf("-\t");
		printf("\n");
	}
}

//In log ra file
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

//Khởi tạo cấu hình
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
  //Gốc của vật thể ở ô giữa hàng cuối
  *(Object + (n - 1) * n + (n / 2)) = 1;
}

//Tính SOR theo thứ tự đỏ đen
void calc_red_back(float *C, float *Cnew, int M, int N, int rb, int rank, float *delta)
{
  for (int i = 1; i <= M; i++)
    for (int j = 0; j < N; j++) {
      int ii = M * rank + i - 1;
      int ij = j;
      if (*(Obj + i * N + j) == 1) {
        *(Cnew + i * N + j) = *(C + i * N + j);
        continue;
      }
      if (rank == 0 && i == 1) {
        *(Cnew + i * N + j) = *(C + i * N + j);
        continue;
      }
      if (rank == NP - 1 && i == M) {
        *(Cnew + i * N + j) = *(C + i * N + j);
        continue;
      }
	  //Trường hợp tổng quát
      if ((ii + ij) % 2 == rb) {
        *(Cnew + i * N + j) = (omega / 4) * (*(C + (i - 1) * N + j) + *(C + (i + 1) * N + j) 
                            + *(C + i * N + ((j - 1 + N) % N)) + *(C + i * N + ((j + 1) % N))) 
                            + (1 - omega) * (*(C + i * N + j));
        if (fabs(*(Cnew + i * N + j) - *(C + i * N + j)) > *delta)
          *delta = fabs(*(Cnew + i * N + j) - *(C + i * N + j));
      } else {
        *(Cnew + i * N + j) = *(C + i * N + j);
      }
    }
}

void diffusion()
{
  do {
    delta = 0.0;
    //Tính các ô đỏ
	//Gửi hàng cuối cho tiến trình phía dưới
    if (rank != NP - 1) {
      MPI_Send(Cstore + mc * n, n, MPI_FLOAT, rank + 1, rank, MPI_COMM_WORLD);
    }
    if (rank != 0) {
      MPI_Recv(Cstore, n, MPI_FLOAT, rank - 1, rank - 1, MPI_COMM_WORLD, &status);
    }

	//Gửi hàng đầu cho tiến trình phía trên
    if (rank != 0) {
      MPI_Send(Cstore + n, n, MPI_FLOAT, rank - 1, rank, MPI_COMM_WORLD);
    }
    if (rank != NP - 1) {
      MPI_Recv(Cstore + (mc + 1) * n, n, MPI_FLOAT, rank + 1, rank + 1, MPI_COMM_WORLD, &status);
    }
    calc_red_back(Cstore, Cnew, mc, n, 0, rank, &delta);
	//Lưu lại các giá trị tính được vào Cstore
    copyArray(Cstore, Cnew, mc, n);
    //Tính các ô đen
	//Gửi hàng cuối cho tiến trình phía dưới
    if (rank != NP - 1) {
      MPI_Send(Cstore + mc * n, n, MPI_FLOAT, rank + 1, rank, MPI_COMM_WORLD);
    }
    if (rank != 0) {
      MPI_Recv(Cstore, n, MPI_FLOAT, rank - 1, rank - 1, MPI_COMM_WORLD, &status);
    }
	
	//Gửi hàng đầu cho tiến trình phía trên
    if (rank != 0) {
      MPI_Send(Cstore + n, n, MPI_FLOAT, rank - 1, rank, MPI_COMM_WORLD);
    }
    if (rank != NP - 1) {
      MPI_Recv(Cstore + (mc + 1) * n, n, MPI_FLOAT, rank + 1, rank + 1, MPI_COMM_WORLD, &status);
    }
    calc_red_back(Cstore, Cnew, mc, n, 1, rank, &delta);
    copyArray(Cstore, Cnew, mc, n);

    //Tính max delta từ các tiến trình
    float new_delta = 0;
    MPI_Reduce(&delta, &new_delta, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
      delta = new_delta;
      // printf("%0.5f\n", delta);
    }
	//Phát trả max delta cho các tiến trình
    MPI_Bcast(&delta, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
  } while (delta > tolerance);
}

int main(int argc, char *argv[])
{
  freopen("config.txt", "r", stdin);
  scanf("%d\n%d\n%f\n%f\n%f", &n, &numIter, &eta, &omega, &tolerance);
  // freopen("out.txt", "w", stdout);
  srand(time(0));

 
  C = (float *)malloc(n * n * sizeof(float));
  Object = (int *) malloc(n * n * sizeof(int));
  Init();
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &NP);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  mc = n / NP;
  //Khai báo bộ nhớ cho Cnew: Cnew gồm mc hàng và thêm 1 hàng trên cùng, 1 dưới cùng là biên
  //Tương tự với Cstore, Obj, candidates
  Cnew = (float *)malloc((mc + 2) * n * sizeof(float));
  Cstore = (float *)malloc((mc + 2) * n * sizeof(float));

  Obj = (int *)malloc((mc + 2) * n * sizeof(int));
  candidates = (int *)malloc((mc + 2) * n * sizeof(int));

  nutriEachProc = (float *)malloc(NP * sizeof(float));
  nutri = (float *)malloc(sizeof(float));
  
  if(rank == 0){
    iteratorLog(0, C, Object);
  }
  //Phân chia dữ liệu cho các tiến trình
  MPI_Scatter(C, mc * n, MPI_FLOAT, Cstore + n, mc * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Scatter(Object, mc * n, MPI_INT, Obj + n, mc * n, MPI_INT, 0, MPI_COMM_WORLD);
  //Start iterating
  for (int k = 1; k <= numIter; k++){
    // Khuếch tán và tìm trạng thái steady
    diffusion();
    
    //Gửi hàng cuối cho tiến tình phía dưới
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

    //Gửi hàng đầu cho tiến tình phía trên
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

    //Khởii tạo candidates
    for (i = 1; i <= mc; i++)
      for (j = 0; j < n; j++)
        *(candidates + i * n + j) = 0;

    *nutri = 0;
    //Xác định candidates
    for (i = 1; i <= mc; i++) {
      for (j = 0; j < n; j++) {
        if (*(Obj + i * n + j) == 1)
          continue;

        int sum = 0;
        //Với mỗi hướng, ta xét xem hàng xóm ở hướng đó có thuộc vật thể không
        for (dir = 0; dir < 4; dir++) {
          int u, v;
          u = i + dx[dir];
          v = j + dy[dir];
          if (u >= 0 && u <= mc + 1 && v >= 0 && v < n && *(Obj + u * n + v) == 1)
            sum += 1;
        }
        //Tồn tại một hàng xóm là vật thể thì ô này thuộc candidates
        if (sum > 0) {
		  //Tính tổng dinh dưỡng của tiến trình này
          *nutri += pow(*(Cstore + i * n + j), eta);
          *(candidates + i * n + j) = 1;
        }
      }
    }

    MPI_Allgather(nutri, 1, MPI_FLOAT, nutriEachProc, 1, MPI_FLOAT, MPI_COMM_WORLD);
    //Tính tổng dinh dưỡng
    float totalNutri = 0.0;
    for (i = 0; i < NP; ++i)
      totalNutri += *(nutriEachProc + i);

    //Phát triển vật thể
    for (i = 1; i <= mc; ++i) {
      for (j = 0; j < n; ++j) {
        if (*(candidates + i * n + j) == 1 && randProb() <= (pow(*(Cstore + i * n + j), eta) / totalNutri)){
          *(Obj + i * n + j) = 1;
          *(Cstore + i * n + j) = 0.0;
        }
      }
    }
	//In log ra file
    if(k % 10 == 0){
      MPI_Gather(Cstore + n, mc * n, MPI_FLOAT, C, mc * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
      MPI_Gather(Obj + n, mc * n, MPI_INT, Object, mc * n, MPI_INT, 0, MPI_COMM_WORLD);
    }
    if(rank == 0 && k % 10 == 0){
      iteratorLog(k, C, Object);
    }
  }
  diffusion();

  MPI_Gather(Cstore + n, mc * n, MPI_FLOAT, C, mc * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Gather(Obj + n, mc * n, MPI_INT, Object, mc * n, MPI_INT, 0, MPI_COMM_WORLD);
  if(rank == 0) {
    printf("gathering\n");
    printArrayFloat(C);
    printArrayInt(Object);
    iteratorLog(numIter, C, Object);
  }
  MPI_Finalize();

  return 0;
}
