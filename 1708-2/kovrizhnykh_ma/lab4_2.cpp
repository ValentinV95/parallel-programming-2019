#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "omp.h"

#define N 1000

int compare(const void *, const void *);
void mergeArrays(int *, int *, int *, int, int);
int computeNeighbor(int, int, int);

int main(int argc, char ** argv)
{
	int i, j, n, rank, size;
	double WTime_Start = 0, WTime_End = 0;

	int numElements = N / size;
	int * arr = (int *)malloc(sizeof(int)*numElements);
	int * temp = (int *)malloc(sizeof(int)*numElements * 2);
	int * recvArr = (int *)malloc(sizeof(int)*numElements);
	int * fullArr = NULL;

	if (rank == 0)
		fullArr = (int *)malloc(sizeof(int)*N);

	//Fill array in descending order
	int start = rank * numElements;
	int end = start + numElements;
	for (i = 0, j = start; i < numElements; i++, j++)
		arr[i] = N - j;

	srand(time(NULL));

#pragma omp single
	{
		WTime_Start = omp_get_wtime();
	}
	
	omp_set_num_threads(rank);
#pragma omp parallel shared(arr)

	//Sort array with odd-even sort

	//Sort local values
	qsort(arr, numElements, sizeof(int), compare);

	//Begin iterations
	for (n = 0; n < size; n++) 
	{
		int neighbor = computeNeighbor(n, rank, size);

		if (neighbor >= 0 && neighbor < size)
		{
		
			//If my rank < my neighbor's rank, keep the smaller values
			if (rank < neighbor) 
			{
				mergeArrays(arr, recvArr, temp, numElements, 1);
				//Else keep the larger values
			}
			else 
			{
				mergeArrays(arr, recvArr, temp, numElements, 0);
			}
		}
	}

	

	if (rank == 0)
	{
		for (i = 0; i < N; i++)
			printf("%d ", fullArr[i]);
		printf("\n");
	}

	free((void *)arr);
	free((void *)recvArr);
	free((void *)temp);

	WTime_End = omp_get_wtime();
	cout << "Time = " << WTime_End - WTime_Start << " sec\n";
	
}

int compare(const void * a, const void * b)
{
	if (*((int *)a) < *((int *)b)) return -1;
	else if (*((int *)a) == *((int *)b)) return 0;
	else return 1;
}

int computeNeighbor(int phase, int rank, int size)
{
	int neighbor;
	if (phase % 2 != 0) {  //odd phase
		if (rank % 2 != 0) {  //odd rank
			neighbor = rank + 1;
		}
		else {  //even rank
			neighbor = rank - 1;
		}
	}
	else {  //even phase
		if (rank % 2 != 0) {  //odd rank
			neighbor = rank - 1;
		}
		else {  //even rank
			neighbor = rank + 1;
		}
	}

	if (neighbor < 0 || neighbor >= size)
		neighbor = -1;
	return neighbor;
}

void mergeArrays(int * arr, int * neighborArr, int * temp, int size, int low_or_high)
{
	int i, j, k;
	i = j = k = 0;

	//Assume arrays are already sorted
	for (i = 0, j = 0, k = 0; i < size * 2; i++)
	{
		if (j < size && k < size)
		{
			if (arr[j] < neighborArr[k])
			{
				temp[i] = arr[j];
				j++;
			}
			else {
				temp[i] = neighborArr[k];
				k++;
			}
		}
		else if (j < size) {
			temp[i] = arr[j];
			j++;
		}
		else {
			temp[i] = neighborArr[k];
			k++;
		}
	}

	if (low_or_high % 2 != 0)
		for (i = 0; i < size; i++)
			arr[i] = temp[i];
	else
		for (i = size, j = 0; j < size; i++, j++)
			arr[j] = temp[i];
}