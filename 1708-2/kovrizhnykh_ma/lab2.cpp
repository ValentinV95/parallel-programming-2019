#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <assert.h>

using namespace std;

/*float *create_rand_nums(int num_elements)	// генератор флотовых элем (0-1)
{
	float *rand_nums = (float *)malloc(sizeof(float) * num_elements);
	
	for (int i = 0; i < num_elements; i++) 
	{
		rand_nums[i] = (rand() / (float)RAND_MAX);
	}

	return rand_nums;
}


float compute_avg(float *array, int num_elements)	// считает среднее
{
	float sum = 0;

	for (int i = 0; i < num_elements; i++) 
	{
		sum += array[i];
	}

	return sum / num_elements;
}*/

void My_MPI_Gather(int* sendbuf, int sendcount, MPI_Datatype d1, int* recvbuf, int recvcount, MPI_Datatype d2, int root, MPI_Comm comm)
{
	int numprocs, myid;
	int *temp = new int[sendcount];
	int temp2;
	MPI_Status status;
	MPI_Comm_size(comm, &numprocs);
	MPI_Comm_rank(comm, &myid);

	if (myid == root)
	{
		for (int i = 0; i < numprocs; i++)
		{
			if (i != root)
			{
				MPI_Recv(sendbuf, recvcount, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, *sendbuf, &status);
				for (int j = 0; j < recvcount; j++)
				{
					recvbuf[recvcount*i + j] = *temp;
				}
			}
			recvbuf[root*recvcount] = *sendbuf;
		}

	}
	else
	{
		MPI_Send(temp, sendcount, d1, root, 99, *recvbuf);
	}
}

/*int main(int argc, char** argv) 
{
	int num_elements_per_proc = atoi(argv[1]);	// ?
	
	srand(time(NULL));	// генератор случ чисел

	MPI_Init(&argc, &argv);

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// созд случ массива в корне (размер = число элем на проце * кол-во процов)
	float *rand_nums = NULL;
	if (world_rank == 0) 
	{
		rand_nums = create_rand_nums(num_elements_per_proc * world_size);
	}

	// созд буфера для кажд проца
	float *sub_rand_nums = (float *)malloc(sizeof(float) * num_elements_per_proc);
	assert(sub_rand_nums != NULL);

	// рассеивание частей на все процы
	MPI_Scatter(rand_nums, num_elements_per_proc, MPI_FLOAT, sub_rand_nums, num_elements_per_proc, MPI_FLOAT, 0, MPI_COMM_WORLD);

	// среднее из частей
	float sub_avg = compute_avg(sub_rand_nums, num_elements_per_proc);

	// сбор средних из частей в корень
	float *sub_avgs = NULL;

	if (world_rank == 0) 
	{
		sub_avgs = (float *)malloc(sizeof(float) * world_size);
		assert(sub_avgs != NULL);
	}

	MPI_Gather(&sub_avg, 1, MPI_FLOAT, sub_avgs, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

	// подчёт общего среднего всех чисел
	if (world_rank == 0) 
	{
		float avg = compute_avg(sub_avgs, world_size);
		cout << "Avg of all elements: " << avg << endl;

		// вычислить среднее по исходным данным для сравнения
		float original_data_avg = compute_avg(rand_nums, num_elements_per_proc * world_size);
		cout << "Avg computed across original data: " << original_data_avg << endl;
	}

	// очистка
	if (world_rank == 0) 
	{
		free(rand_nums);
		free(sub_avgs);
	}

	free(sub_rand_nums);

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
}*/
