
#include "stdafx.h"


#include <omp.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

void monteCarlo()
{
	// circle inscribed the square ,circle centre(0,0) and square verices{(1,-1),(1,1),(-1,1),(-1,-1)} 
	//so the point generted random should be in the range of sqaure.
	long circle_points = 0, n1 = 0;
	double count = 100000, pi, start_t, end_t;
	double x = 0, y = 0;
	double dis;
	srand(time(0));
	long i = 0;
	start_t = omp_get_wtime();
	for (i = 0; i<count; i++)
	{
		x = (double)rand() / RAND_MAX; // value be <1 (not taking -ve half for bith so no prblm)
		y = (double)rand() / RAND_MAX;
		dis = x*x + y*y;
		if (dis <= 1)
			circle_points++;

	}
	pi = (double)(4 * circle_points) / count;  // total point bcoz we have taken points inside square only

	end_t = omp_get_wtime();
	printf("pi=%.15lf and Execution time having %ld threads is %.15lf\n", pi, n1, end_t - start_t);
	// parallel 
	i = 0;
	pi = 0;
	circle_points = 0;
	omp_set_num_threads(16);
	start_t = omp_get_wtime();
#pragma omp parallel
	{
		int ID = omp_get_thread_num();
		int n = omp_get_num_threads();
#pragma omp critical
		for (i = ID; i< count; i = i + n) // **reductn loop will do by itself parallelism no need to assign by us.
		{
			x = (double)rand() / RAND_MAX;
			y = (double)rand() / RAND_MAX;
			dis = x*x + y*y;
			if (dis <= 1)
				circle_points++;
		}
		n1 = n;
		// #pragma atomic
		// pi = (double)(4 * circle_points) / count;
	}
	pi = (double)(4 * circle_points) / count;
	end_t = omp_get_wtime();
	printf("pi=%.15lf and Execution time having %ld threads is %.15lf\n", pi, n1, end_t - start_t);

}
int main()
{
	monteCarlo();

	getchar();
	return 0;
}