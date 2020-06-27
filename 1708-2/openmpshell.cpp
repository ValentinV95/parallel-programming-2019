#include "pch.h"
#include <iostream>
#include "malloc.h"
#include "omp.h"
#include <time.h>
#include <ctime>
#include <cstdlib>
#define MAX_SIZE 1000
using namespace std;

//a[] is sort , n is size

void merge(int a[], int n, int t[]) {
	int i = 0;
	int j = n / 2;
	int ti = 0;

	while (i < n / 2 && j < n) {
		if (a[i] < a[j]) {
			t[ti] = a[i];
			ti++; i++;
		}
		else {
			t[ti] = a[j];
			ti++; j++;
		}
	}
	while (i < n / 2) 
	{ 
		t[ti] = a[i];
		ti++; i++;
	}
	while (j < n) 
	{ 

		t[ti] = a[j];
		ti++; j++;
	}
	memcpy(a, t, n * sizeof(int));

}





void ShellSort(int a[], int n)
{
	int gap;
	int j;
	int i;
	int element;

	for (gap = n / 2; gap > 0; gap /= 2)
	{
#pragma omp parallel for shared( a, gap,n) private(i) default(none)
		for (i = gap; i < n; i++)
		{
#pragma omp parallel
			element = a[i];
			for (j = i; j >= gap; j -= gap)
			{
#pragma omp critical
				if (element < a[j - gap])
					a[j] = a[j - gap];

			}

			a[j] = element;

		}
	}
}

void mergeSort(int a[], int t[], int n)
{
#pragma omp parallel sections
	if (n > 1) {
#pragma omp section
		for (int i = 0; i < n / 2; i++) {
			t[i] = a[i];
		}
#pragma omp task firstprivate(a,n,t)
		mergeSort(a, t, n / 2);
#pragma omp section
		for (int i = n / 2; i < n; i++) {
			t[i] = a[i];
		}
#pragma omp task firstprivate (a, n, t)
		mergeSort(a, t, n / 2);
#pragma omp taskwait

#pragma omp task firstprivate (a, n)
		ShellSort(a, n);
	
	
	}
}



void print(int a[], int size)
{

	for (int i = 0; i < size; i++)
		cout << a[i] << "";
	cout << endl;
}



int main()

{
	int threads = 2;
	int  t[MAX_SIZE];
	int n = 2000;
	int a[2000];
	int i;
	srand((time(NULL)));
	omp_set_nested(1);
#pragma omp parallel for num_threads(i)
	for (i = 0; i < n; i++)
	{
		a[i] = (rand() % 100000) + 1;

	}

	double startTime = clock();

	cout << "enter element:" << endl;
	cin >> n;
	cout << "enter elements the one that you put earlier:" << endl;
	for (int i = 0; i < n; i++)
	{
		cin >> a[i];

	}
	cout << "array seq before sorting: ";

	print(a, n);
	ShellSort(a, n);

	cout << "array seq after sorting: ";
	print(a, n);
	omp_set_num_threads(threads);

	merge(a,n+1,t);
	mergeSort(a, 0,n-1);

	cout << "\tSorted Array Elements with shell" << endl;
	for (int i = 0; i < n; i++) {
		cout << a[i] << "\t";
	}


	ShellSort(a, n);
	double endTime = clock();
	int totalTime = endTime - startTime; // The average time to run this

	cout << "This is the time it took to run.\n" << endl;
	cout << totalTime / 2000 << n << endl;





}