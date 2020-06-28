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
#include <ctime>
#include <algorithm>
#include <utility>
using namespace tbb;

#define MAX_SIZE 1000


//a[] is sort , n is size

void print(int a[], int n) {
	for (int i = 0; i < n; i++)
		std::cout << a[i] << " ";
	std::cout << std::endl;
}

void merge(int a[], int n, int gap) {
	for (int j = gap; j < n; j += gap) {
		int key = a[j];
		int i = j - gap;
		while (i >= 0 && a[i] > key) {
			a[i + gap] = a[i];
			i -= gap;
		}
		a[i + gap] = key;
	}
}

void shellsort(int a[], int n) {
	for (int gap = n / 2; gap > 0; gap /= 2)
		tbb::parallel_for(tbb::blocked_range<int>(0, gap),
			[&](const tbb::blocked_range<int> &r) {
				for (int i = r.begin(); i < r.end(); i++) {
					merge(a + i, n - i, gap);
				}
			});
}

int main() {
	int n = 2000;
	int a[2000];

	
	const int threadNum = 2;

	tbb::tick_count startTime;
	double maintime;
	double defineTime;
	double parallelTime;

	srand((time(NULL)));


	for (int i = 0; i < n; i++)
	{
		a[i] = (rand() % 100000) + 1;

	}

	
	std::cout << "enter elements :";
	
	std::cin >> n;
	std::cout << "enter elements the one that you put earlier:";
	
	for (int i = 0; i < n; i++)
	{
		std::cin >> a[i];

	}
	std::cout << "array seq before sorting: ";
	
	
	print(a, n);
	
	
	startTime = tbb::tick_count::now();
	
	maintime = (tbb::tick_count::now() - startTime).seconds();

	tbb::task_scheduler_init init(1);
	startTime = tbb::tick_count::now();

	shellsort(a, n);

	defineTime = (tbb::tick_count::now() - startTime).seconds();
	init.terminate();

	init.initialize(threadNum);
	startTime = tbb::tick_count::now();

	shellsort(a, n);
	parallelTime = (tbb::tick_count::now() - startTime).seconds();

	init.terminate();

	if (n < 100) {
		std::cout << "Sorted array: ";
		print(a, n);
	}
	shellsort(a, n);
	
		std::cout << "\nParallel shell sort " << std::endl;

		std::cout << "maintimestarting: " << maintime << std::endl;

		std::cout << "\ndefine time:  " << defineTime << std::endl;

			std::cout << "\nparallel time: " << parallelTime << std::endl;

			std::cout << "\nparallel efficencty= ";
		std::cout << defineTime / parallelTime << std::endl;

	
	return 0;
}




