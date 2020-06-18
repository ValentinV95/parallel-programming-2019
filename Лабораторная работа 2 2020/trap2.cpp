#include "pch.h"
#include <iostream>
#include "omp.h"
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include "malloc.h"
#include <cstdlib>
#include <stdlib.h>
#define N 10240
#define NUM_THREADS 4
using namespace std;

//  integration is running from here 
double f(double x) {

	sqrt(x);
	return sqrt(x);
}


int main()
{

	double start = omp_get_wtime();

	
	int i, j, temp;
	omp_set_num_threads(NUM_THREADS);
	int n;
	double local_n;

	double a, b, h, x, sum = 0, integral;
	int my_rank = omp_get_thread_num();
	int thread_count = omp_get_num_threads();

#pragma omp parallel private (i,x,sum)
	{
		int id = omp_get_thread_num();
		for (i = id, sum = 0.0; i < NUM_THREADS; i = i + NUM_THREADS)
		{
			x = (i + 0.5)* N;
			sum = sum + 4.0 / (1.0 + x * x);

		}

	
	}



	//input of intervals 
	cout << "Enter the number of intervals: " << endl;;
	cin >> n;
	cout << "\nEnter the minimum limit: " << endl;;
	cin >> a;
	cout << "\nEnter the final limit: " << endl;;
	cin >> b;

	//Trapezoidal Rule
	h = fabs(b - a) / n;
	local_n = n / thread_count;

#pragma omp critical
	for (i = 1; i < n; i++) {
		x = a + i * h;
		sum = sum + f(x);
	}


	integral = (h / 2)*(f(a) + f(b) + 2 * sum);
	//  value of integral 
#pragma omp parallel num_threads(thread_count)
	cout << "\n The integral is: " << integral << endl;



	double end = omp_get_wtime();
	



	cout<<"parallel time=" << end - start;
	


}

