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

using namespace tbb;

class ShellSort  {
	int a[1000], n;


public :


	  
		void merge();
		void print();
		void sortmain();
		void test_threading_building_blocks();
	
};



void ShellSort::merge()
{
	std::cout << "type element: ";
	std::cin >> n;
	for (int i = 0; i < n; i++)
		std::cin >> a[i];

	if (n > 1) {
		for (int i = 0; i < n / 2; i++) {
			a[i] = a[i];
		}


	}



}



void ShellSort::print() {
	std::cout << "\n_print data_\n";
	for (int i = 0; i < n; i++)
		std::cout << a[i];
	std::cout << "  ";
}


//t(temp) == element
void ShellSort:: sortmain() {
	int i;
	int j;
	int element;
	int gap;

	for (gap = n / 2; gap > 0; gap /= 2) {
		for (i = gap; i < n; i++) {
			element = a[i];
			for (j = i; j >= gap; j -= gap) {
				if (element < a[j - gap])
					a[j] = a[j - gap];
				else
					break;
			}
			a[j] = element;
		}
	}
}
//tbb code is taken from thomas kim video tutorial adn re edited
void test_threading_building_blocks()
{
	std::mutex mutex;
	using lock_type = std::lock_guard<std::mutex>;

	auto callback = [&mutex](auto index)
	{
		lock_type lock(mutex);
		std::cout << "thread Id"<< &ShellSort::sortmain <<std::this_thread::get_id()
			<< " - index = " << index << std::endl;

	};


	tbb::parallel_for(1, 11, callback);
}








int main() {
	int n = 2000;
	int a[2000];
	


	double startTime = clock();

	std::cout << "enter element:";
	std::cin >> n;
	std::cout << "enter elements the one that you put earlier:";
	for (int i = 0; i < n; i++)
	{
		std::cin >> a[i];

	}
	std::cout << "array seq before sorting: ";
	test_threading_building_blocks();

	//std::cout << "\t0___0\t";

	ShellSort t;
	t.merge();
	t.print();
	t.sortmain();
	



	double endTime = clock();
	double totalTime = endTime - startTime; // The average time to run this

	std::cout << "This is the time it took to run.\n";
	std::cout << totalTime / 2000 << n;

	


	



}