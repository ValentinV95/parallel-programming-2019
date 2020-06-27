// tbbone.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//clang++ -std=c++17 -g -DTBB_USE_DEBUG=1 tbbone.cpp -ltbb_debug -ltbbmalloc_debug -o tbbone.exe
//clang++ -std=c++17 tbbone.cpp -ltbb -ltbbmalloc


#include "pch.h"
#include <iostream>
#include "malloc.h"
#include <time.h>
#include <cstdlib>
#include <stdlib.h>

#include "tbb/tbb.h"
#include <thread>
#include <mutex>
#include "tbb/parallel_sort.h"

using namespace tbb;

#define MAX_SIZE 1000


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
		tbb::parallel_for(tbb::blocked_range<int>(0, gap),
			[&](const tbb::blocked_range<int> &r) {
				for (int i = r.begin(); i < r.end(); i++) {
					merge(a + i, n - i, 0);
				}
			});

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

void mergeSort(int a[], int t[], int n)
{

	if (n > 1) {

		for (int i = 0; i < n / 2; i++) {
			t[i] = a[i];
		}

	

		for (int i = n / 2; i < n; i++) {
			t[i] = a[i];
		}
		


		mergeSort(a, t, n / 2);

		ShellSort(a, n);


	}
}




void print(int a[], int size)
{

	for (int i = 0; i < size; i++)
		std::cout << a[i];
	std::cout<<"";
}





int main(int argc, char *argv[])

{
	int threads=2;
	
	int  t[MAX_SIZE];
	int n =2000;
	int a[2000];
	int i;
	

	srand((time(NULL)));


	for (i = 0; i < n; i++)
	{
		a[i] = (rand() % 100000) + 1;

	}
	

	tbb::tick_count start= tbb::tick_count::now();
	tbb::tick_count stop = tbb::tick_count::now();

	double maintimetbb, ParallelTime,ParallelTimeformerge;



	std::cout << "enter element:";
	std::cin >> n;
	std::cout << "enter elements the one that you put earlier:";
	for (int i = 0; i < n; i++)
	{
		std::cin >> a[i];

	}
	std::cout << "array seq before sorting: ";
	print(a, n);

	std::cout << "\n maintimetbb " <<
		(maintimetbb = ((tbb::tick_count::now()) - start).seconds());
	start = tbb::tick_count::now();

	


	
	//ShellSort(a, n);

	std::cout << "\narray seq after sorting: ";
	print(a, n);

	

	


	std::cout << "\nenter the size of the array";
	std::cin >> n;
	for (int i = 0; i < n; i++)
	{
		std::cin >> a[i];

	}
	


	std::cout << "\narray seq before sorting: ";

	print(a, n);

	
	for (int i = 0; i < n; i++) {
		std::cout << "\nenter the element of shell:" << i;
		std::cin >> a[i];
	}
	print(a, n);

	std::cout << "\nSorted Array Elements with shell:";
	for (int i = 0; i < n; i++) {
		std::cout << a[i] << "\t";
	}

	
	
	//mergeSort(a, t + 1, n);
	ShellSort(a, n);
	mergeSort(a, 0, n - 1);

	parallel_sort(a, a + n);
	std::cout << "\nParallelTime: ";
	(ParallelTime = ((tbb::tick_count::now()) - stop).seconds());
	stop = tbb::tick_count::now();


	std::cout << (maintimetbb / ParallelTime ) << "\nmaintimetbb: ";
	
	ShellSort(a, n);
	print(a, n);




}








