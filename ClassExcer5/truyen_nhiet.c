#include <stdio.h>
#include <omp.h>

#define M 20
#define Time 1
#define dt 0.01
#define dx 0.1
#define D 0.1

// Tuan tu
/*void DHB2(float *T, float *dT)
{
	int i;
	float c, l, r;
	for (i = 0; i < M; i++)
	{
		c = T[i];
		l = (i == 0) ? 100 : T[i - 1];
		r = (i == M - 1) ? 25 : T[i + 1];
		dT[i] = D * (l - 2 * c + r) / (dx * dx);
	}
}

int main()
{
	int i;
	float T[M], dT[M], t;

	// Khoi tao
	for (i = 0; i < M; i++)
	{
		T[i] = 25;
	}

	// Mo phong
	printf("t = %5.2f, T = {", 0.0);
	for (i = 0; i < M; i++)
	{
		printf("%5.2f, ", T[i]);
	}
	printf("\b\b}\n");

	for (t = dt; t <= Time; t = t + dt)
	{
		DHB2(T, dT);
		printf("t = %5.2f, T = {", t);
		for (i = 0; i < M; i++)
		{
			T[i] = T[i] + dT[i] * dt;
			printf("%5.2f, ", T[i]);
		}
		printf("\b\b}\n");
	}

	return 0;
}*/

// Song song
void DHB2(float *T, float *dT)
{
	int i, id;
	float c, l, r;
	
	#pragma omp parallel private(i, id, c, l, r)
	for (i = 0; i < M; i++)
	{
		c = T[i];
		l = (i == 0) ? 100 : T[i - 1];
		r = (i == M - 1) ? 25 : T[i + 1];
		dT[i] = D * (l - 2 * c + r) / (dx * dx);
	}
}

int main()
{
	int i;
	float T[M], dT[M], t;

	// Khoi tao
	for (i = 0; i < M; i++)
	{
		T[i] = 25;
	}

	// Mo phong
	printf("t = %5.2f, T = {", 0.0);
	for (i = 0; i < M; i++)
	{
		printf("%5.2f, ", T[i]);
	}
	printf("\b\b}\n");

	for (t = dt; t <= Time; t = t + dt)
	{
		DHB2(T, dT);
		printf("t = %5.2f, T = {", t);
		for (i = 0; i < M; i++)
		{
			T[i] = T[i] + dT[i] * dt;
			printf("%5.2f, ", T[i]);
		}
		printf("\b\b}\n");
	}

	return 0;
}
