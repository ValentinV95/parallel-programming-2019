#include "pch.h"
#include <iostream>
#include "omp.h"
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include "malloc.h"
#include <cstdlib>
#include <stdlib.h>
#include <cmath>


double n;

#define NUM_THREADS 2

using namespace std;

float f(float x) {

	return  x * x ;

}

float rect(float local_a, float local_b, float local_n, float(*f)(float))
{
	float local_integral;
	float x;
	int i;
	float local_h;



	local_h = (local_b - local_a) / local_n;
	local_integral = f(local_a);
	x = local_a;

#pragma omp parallel private (i,n,local_n)
	for (i = 1; i <= local_n - 1; i++)
	{
#pragma omp sections
		x = x + local_h;
		local_integral += f(x);
	}
#pragma omp critical
	local_integral *= local_h;
#pragma omp sections
	return local_integral;
}

int main()
{
	clock_t t1, t2;
	double x, sum;


	
	omp_set_num_threads(NUM_THREADS);


	t1 = clock();

	sum = 0;
	for (int i = 0; i < NUM_THREADS; i++)
	{
		x = (i + 0.5)*n;
		sum = sum + 4.0 / (1.0 + x * x );
	}
#pragma omp critical
	x = n * NUM_THREADS;

	// Integral of rect of x^0 over (1.0,2.0);

	cout << rect << 1.0, 2.0, 200000;
	cout << f;

	



	t2 = clock();
	

	cout << "\npara time :"<< t2 - t1;





	return 0;
}