#include <stdio.h>
#include "pch.h"
#include <mpi.h>

int main(int argc, char** argv)
{
	int const n = 1000; 
	int a[n];
	int i, sum, ssum, iproc, nproc;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &iproc);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	for (i = 1 + iproc; i <= n; i = i + nproc) {
		a[i - 1] = i;
	}
	sum = 0;
	for (i = 1 + iproc; i <= n; i = i + nproc) {
		sum = sum + a[i - 1];
	}
	MPI_Reduce(&sum, &ssum, 1, MPI_INTEGER, MPI_SUM, 0, MPI_COMM_WORLD);
	sum = ssum;
	if (iproc == 0) {
		printf("sum %d\n ", sum);

	}
	MPI_Finalize();
	return 0;
}
