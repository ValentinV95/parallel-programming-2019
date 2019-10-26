#include "mpi.h"
#include <iostream> 
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define M 5 
#define N 5 

using namespace std;


void fill_matrix_randomly(int matrix[N][M], int max_value);
void write_matrix(int matrix[N][M]);

void transpose(int matrix[N][M]);

int find_min(int* vector, int vector_size);
void write_vector(int* vector, int vector_size);

int main(int argc, char* argv[])
{
	int my_rank = 0;
	int comm_size = 0;

	int A[N][M];

	int receive_buffer[M];

	int partial_min[M];

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

	if (comm_size != N)
	{
		printf("Please set process count = %d and run again.", N);
		MPI_Finalize();
		return 0;
	}

	if (my_rank == 0)
	{
		printf("\n");
		fill_matrix_randomly(A, 20);
		write_matrix(A);
		transpose(A);
	}

	MPI_Scatter(A, M, MPI_INT, receive_buffer, M, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Reduce(receive_buffer, partial_min, M, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

	if (my_rank == 0)
	{
		printf("\n[*]Vector of partial min values.\n\n");
		write_vector(partial_min, M);
		int min = find_min(partial_min, M);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();

	return 0;
}
/*-----------------------fill_matrix_randomly-------------------------*/
void fill_matrix_randomly(int matrix[N][M], int max_value)
{
	int i = 0;
	int j = 0;
	srand(time(NULL));
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < M; j++)
		{
			matrix[i][j] = rand() % max_value + 1;
		}
	}
}
/*--------------------------write_matrix----------------------------*/
void write_matrix(int matrix[N][M])
{
	int i = 0;
	int j = 0;
	for (i = 0; i < N; i++)
	{
		if (i == 0 || i == (N - 1))
			printf("+");
		else
			printf("|");
		for (j = 0; j < M; j++)
		{
			if (j>0 && j<M)
				printf("|");
			printf("%4d  ", matrix[i][j]);
		}
		if (i == 0 || i == (N - 1) && j == (N - 1))
			printf("+\n");
		else
			printf("|\n");
	}
}
/*-----------------------------transpose-------------------------------*/
void transpose(int matrix[N][M])
{

	for (int i = 0; i < M; i++)
	for (int j = i + 1; j < N; j++)
		swap(matrix[i][j], matrix[j][i]);

	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < N; j++)
			matrix[i][j];
	}
}
/*--------------------------write_vector----------------------------*/
void write_vector(int* vector, int vector_size)
{
	int i = 0;
	for (i = 0; i < vector_size; i++)
	{
		printf("[+]The minimum values on the row[%d] = %d\n", i, vector[i]);
	}
	printf("\n");
}

int find_min(int* vector, int vector_size)
{
	int min = vector[0];
	int i = 0;
	for (i = 0; i < vector_size; i++)
	{
		if (vector[i] < min)
		{
			min = vector[i];
		}
	}
	return min;
}