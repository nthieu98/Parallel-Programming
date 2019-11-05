#include<stdio.h>
#include<malloc.h>
#include<omp.h>
#define M 20
#define Time 1
#define dt 0.01
#define dx 0.1
#define D 0.1
void daohamBacHai(float *T,float *dT)
{
    float c,l,r;
    int id,i;
    omp_set_num_threads(4);
    #pragma omp parallel private(c,l,r,id,i)
    {
        id = omp_get_thread_num();
        for(i = M*id/4;i<M*(id+1)/4&&i<M-1;i++)
        {
            c = *(T+i);
            l = (i==0)?100:*(T+i-1);
            r = (i==M-1)?25:(*(T+i+1));
            *(dT+i) = D*(l-2*c+r)/(dx*dx);
        }
    }
}
void DisplayArr(float *T,int m)
{
    for (int i = 0; i < m; i++)
    {
        printf("%f ",*(T+i));
    }
}
void Khoitao(float *T)
{
    for (int i = 0; i < M; i++)
    {
        *(T+i) = 25.0;
    }
}
int main()
{
    float *T = (float *)malloc(M*sizeof(float));
    float *dT = (float *)malloc(M*sizeof(float));
    Khoitao(T);
    int Ntime = Time/dt;
    for (int  t = 0; t < Ntime; t++)
    {
       // daohamBacHai(T,dT);
        //printf("\n");
       // DisplayArr(T,M);
        float c,l,r;
        int id,i;
       // omp_set_num_threads(4);
        //int i ,id;
        omp_set_num_threads(4);
        #pragma omp parallel private(i,id,l,c,r)
        {

            id = omp_get_thread_num();
            for(i = id*M/4;i<(id+1)*M/4;i++)
            {
                 c = *(T+i);
                 l = (i==0)?100:*(T+i-1);
                 r = (i==M-1)?25:(*(T+i+1));
                *(dT+i) = D*(l-2*c+r)/(dx*dx);
                *(T+i) += *(dT+i)*dt;
            }
        }
       // t = t +dt;
    }
    #pragma omp barrier
    DisplayArr(T,M);

}