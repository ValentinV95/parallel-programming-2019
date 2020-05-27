#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

#define MASTER 0
#define REPETITIONS 10

double Function(double x);

int main(int argc, char *argv[]){
	int myrank, nprocs, a, b, i;
	long n, j;
	double times[REPETITIONS];
	double time, slowest, startInt, endInt, result, h, x, endresult;


	MPI_Status recv_status;
	MPI_Request request;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	if (argc != 4){
		printf("Usage: mpiexec -n nodecount simpson1c a b n\n\n");
		return 1;
	}

	a = atoi(argv[1]);
	b = atoi(argv[2]);
	n = atol(argv[3]);

	startInt = (double)(b - a) / nprocs * myrank + a;
	endInt = (double)(b - a) / nprocs * (myrank + 1.0) + a;


	for (i = 0; i < REPETITIONS; ++i){
		MPI_Barrier(MPI_COMM_WORLD);
		time = MPI_Wtime();

		h = ((endInt - startInt) / n);
		x = startInt;
		result = Function(x);
		x = x + h;
		for (j = 1l; x < endInt; ++j){
			result = (j % 2 == 0) ? result + 2 * Function(x) : result + 4 * Function(x);
			x = x + h;
		}
		result += Function(x);
		result = result / (3.0) * h;

		MPI_Reduce(&result, &endresult, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

		time = MPI_Wtime() - time;
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Reduce(&time, &slowest, 1, MPI_DOUBLE, MPI_MAX, MASTER, MPI_COMM_WORLD);

		if (myrank == MASTER) {
			times[i] = slowest;
		}
	}

	if (myrank == MASTER){
		double min = times[0];
		for (int i = 1; i < REPETITIONS; i++) {
			min = (times[i] < min) ? times[i] : min;
		}
		//printf("\n%d; %ld; %f; %f\n", nprocs, n, min, endresult);
		printf("\nnprocs= %d; n= %ld; min= %f; endresult= %f\n\n", nprocs, n, min, endresult);
	}

	MPI_Finalize();
}

double Function(double x){
	double result = sqrt(1.0 - x*x);
	if (isnan(result)){ // sometimes encountert a problem with x=1.0 which resulted in minus zero.
		result = 0.0f;
	}
	return result;
}