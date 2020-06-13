#include "pch.h"
#include <iostream>
#include "malloc.h"
#include "omp.h"
#include <ctime>
#include <cstdlib>
#include <thread>
#include <csignal>

class ShellSort {
	int a[1000], n;


public:
	

    void merge();
	void print();
	void sortmain();
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
void ShellSort::sortmain() {
	int i;
	int j;
	int element;
	int gap;

	for (gap = n / 2; gap > 0; gap /= 2) {
		for (i = gap; i < n; i++) {
			element = a[i];
			for ( j = i; j >= gap; j -= gap) {
				if (element < a[j - gap])
					a[j] = a[j - gap];
				else
					break;
			}
			a[j] = element;
		}
	}
}

int main() {

	std::cout << "\t0___0\t" ;
	ShellSort t;
	t.merge();
	t.print();
	t.sortmain();
	
	ShellSort s;

	std::thread t1(&ShellSort::merge, &s);
	std::thread t2(&ShellSort::sortmain, &s);
	std::thread t3(&ShellSort::print, &s);

	t1.join();
	t2.join();
	t3.join();

	system("pause");

	return 0;


}
