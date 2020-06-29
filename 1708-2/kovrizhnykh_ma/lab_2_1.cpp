#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

using namespace std;

#define X 200

void quicksort(int *, int, int);
int partition(int *, int, int);
int choosePivot(int *, int, int);
void swap(int *, int *);

int main(void) {
	srand(time(NULL));

	int size = 20000;
	int threads = 4;

	double* seq = new double[size];
	
	srand(123456);

	int * arr = (int *)malloc(sizeof(int)*size);
	int i;
	for (i = 0; i < size; i++)
		arr[i] = rand() % X;

	for (i = 0; i < size; i++)
		printf("%d ", arr[i]);

	cout << endl << "--------------" << endl;

	clock_t start = clock();
	quicksort(arr, 0, size - 1);
	clock_t end = clock();
	float seconds = (float)(end - start) / CLOCKS_PER_SEC;

	for (i = 0; i < size; i++)
		printf("%d ", arr[i]);

	cout << endl;
	cout << "Time of sort = " << seconds << endl;

	free((void *)arr);

	system("pause");
	
	return 0;	
	
}

void quicksort(int * arr, int lo, int hi)
{
	if (lo < hi)
	{
		int p = partition(arr, lo, hi);
		quicksort(arr, lo, p - 1);
		quicksort(arr, p + 1, hi);
	}
}

int partition(int * arr, int lo, int hi)
{
	int i;
	int pivotIdx = choosePivot(arr, lo, hi);
	int pivotVal = arr[pivotIdx];

	swap(&arr[pivotIdx], &arr[hi]);

	int storeIdx = lo;

	for (i = lo; i < hi; i++)
	{
		if (arr[i] < pivotVal)
		{
			swap(&arr[i], &arr[storeIdx]);
			storeIdx++;
		}
	}

	swap(&arr[storeIdx], &arr[hi]);
	return storeIdx;
}

void swap(int * x, int * y)
{
	int temp = *x;
	*x = *y;
	*y = temp;
}

int choosePivot(int * arr, int lo, int hi)
{
	int mid = (lo + hi) / 2;

	int temp;
	if (arr[lo] > arr[hi])
	{
		temp = lo;
		lo = hi;
		hi = temp;
	}
	if (arr[mid] < arr[lo])
	{
		temp = mid;
		mid = lo;
		lo = temp;
	}
	if (arr[hi] < arr[mid])
	{
		temp = mid;
		mid = hi;
		hi = temp;
	}
	return mid;
}
