#include "pch.h"
#include "mpi.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <ctime>
#include <cmath>
#include <string>
using namespace std;

bool TestCube(MPI_Comm New_Comm, int ndims, int SizeNodeHyperCube, int local_data) {
	int rankProc, sizeProc;
	int pivot = 1;
	int left, right;
	int global_data = 0;
	MPI_Comm_rank(New_Comm, &rankProc);
	MPI_Comm_size(New_Comm, &sizeProc);
	MPI_Bcast(&local_data, 1, MPI_INT, 0, New_Comm);
	MPI_Reduce(&local_data, &global_data, 1, MPI_INT, MPI_SUM, 0, New_Comm);
	if (rankProc == 0)
		cout << global_data; fflush(stdout);;
	return global_data;
}

int main(int argc, char** argv) {
	MPI_Comm Old_Comm = MPI_COMM_WORLD, New_Comm;
	int ProcSize, reorder = 1, ProcRank;
	int local_data = 1, ndims = 4, SizeNodeHyperCube = 2, count_dim;
	int* dim_size, *periods, *coord;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	if (ProcRank == 0) {
		 local_data = atoi(argv[1]);
		 ndims = atoi(argv[2]);
		 SizeNodeHyperCube = atoi(argv[3]);
	}
	 dim_size = new int[ndims];
	 periods = new int[ndims];
	 coord = new int[ndims];
	for (int i = 0; i < ndims; i++) {
		dim_size[i] = SizeNodeHyperCube;
		periods[i] = 1;
	}
	MPI_Cart_create(MPI_COMM_WORLD, ndims, dim_size, periods, reorder, &New_Comm);
	MPI_Cartdim_get(New_Comm, &count_dim);
	MPI_Cart_get(New_Comm, ndims, dim_size, periods, coord);
	for (int i = 0; i < ndims; i++) {
		if (periods[i] != 1) {
			return false;
		}
	}
	bool TestSum = TestCube(New_Comm, ndims, SizeNodeHyperCube, local_data);
	MPI_Finalize();
	delete[] dim_size;
	delete[] periods;
	return 0;
}