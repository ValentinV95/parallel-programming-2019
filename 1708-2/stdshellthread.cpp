
//COPYRIGHT UTSHO_MOMEN
#include "pch.h"
#include <cstring>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include "malloc.h"
#include <ctime>
#include <cstdlib>
#include <thread>
#include <csignal>


#define MAX_SIZE 1000



void ShellSort(int a[], int n)
{
	int gap;
	int j;
	int i;
	int element;

	for (gap = n / 2; gap > 0; gap /= 2)
	{

		for (i = gap; i < n; i++)
		{

			element = a[i];
			for (j = i; j >= gap; j -= gap)
			{

				if (element < a[j - gap])
					a[j] = a[j - gap];

			}

			a[j] = element;

		}
	}
}



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


void print(int a[], int size)
{

	for (int i = 0; i < size; i++)
		std::cout << a[i] << "";
	std::cout << std::endl;
}



void mergeSort(int a[], int t[], int n)
{

	if (n > 1) {

		for (int i = 0; i < n / 2; i++) {
			t[i] = a[i];
		}

		mergeSort(a, t, n / 2);

		for (int i = n / 2; i < n; i++) {
			t[i] = a[i];
		}

		mergeSort(a, t, n / 2);



		ShellSort(a, n);
		merge(a, 0, n + t);

	}
}


void ShellSort_parallel(int a[], int n, int part) {
	int threads = n / part;
	pthread_t thread[threads];
	for (int i = 0; i < threads; i++) {
		print(a + i * part, part);
		ShellSort(a + i * part, part);
		

	}
}


int main()

{
	int threads = 2;

	int  t[MAX_SIZE];
	int n = 2000;
	int a[2000];
	int i;
	srand((time(NULL)));

	for (i = 0; i < n; i++)
	{
		a[i] = (rand() % 100000) + 1;

	}

	double startTime = clock();

	std::cout << "enter element:" << std::endl;
	std::cin >> n;
	
	std::cout << "enter elements the one that you put earlier:" << std::endl;
	for (int i = 0; i < n; i++)
	{
		std::cin >> a[i];
	}
	std::cout << "array seq before sorting: ";
	print(a, n);
	ShellSort(a, n);
	std::cout << "array seq after sorting: ";
	print(a, n);


	print(a, n);
	int part = n / threads;

	std::cout << "thread:\t ";
	ShellSort_parallel(a, n, part);

	print(a, n);





	std::cout << "enter the size of the array" << std::endl;
	std::cin >> n;

	std::cout << "please enter the elements of the array" <<std:: endl;
	for (int i = 0; i < n; i++) {
		std::cout << "enter the element of shell:" << i << std::endl;
		std::cin >> a[i];
	}
	print(a, n);
	ShellSort(a, n);
	mergeSort(a, 0, n - 1);

	std::cout << "\tSorted Array Elements with shell" << std::endl;
	for (int i = 0; i < n; i++) {
		std::cout << a[i] << "\t";
	}


	ShellSort(a, n);

	//threads

	print(a, n);
	int part = n / threads;
	if (n < 100) {
		std::cout << "Sorted array:\t ";
		ShellSort_parallel(a, n, part);
	}
	print(a, n);




	double endTime = clock();
	int totalTime = endTime - startTime;

	std::cout << "This is the time it took to run.\n" <<std:: endl;
	std::cout << totalTime / threads << n << std::endl;


}