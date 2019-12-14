#include "pch.h"
#include "mpi.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <ctime>
#include <cmath>
#include <string>
#include <bitset>
using namespace std;

int recr(int ProcSize, int ld, int neproc) {

	return ld;
}

void sendr(int ProcSize, int ld, int neproc,MPI_Comm New_Comm) {
	if (neproc > 7)
		throw - 1;
	int buffer[10]{1}, buffer2;
	std::bitset<3> b1(neproc);
	std::bitset<3> b2(0);
	std::bitset<3> b3;
	MPI_Status status;
	int i = 0, j = 0;
	for (int j = 0; j < 8; j++) {
		if (i < 3) {
			if (b2[i] != b1[i]) {
				b3 = b2;
				b2[i] = b1[i];
				cout << (int)(b3.to_ulong()) << "at"; fflush(stdout);;
				cout << (int)(b2.to_ulong()) << "to"; fflush(stdout);;
				MPI_Sendrecv_replace(&buffer, 1, MPI_INT, (int)(b2.to_ulong()), j, (int)(b3.to_ulong()), j, New_Comm, &status);
			}
			i++;
		}
	}
	//int lc = recr(ProcSize, ld, neproc);
}

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
	int local_data = 0, ndims = 0, SizeNodeHyperCube = 0, count_dim;
	int* dim_size, *periods, *coord;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	local_data = atoi(argv[1]);
	ndims = atoi(argv[2]);
	SizeNodeHyperCube = atoi(argv[3]);

	int neproc = atoi(argv[4]);

	dim_size = new int[ndims];
	periods = new int[ndims];
	coord = new int[ndims];
	for (int i = 0; i < ndims; i++) {
		dim_size[i] = SizeNodeHyperCube;
		periods[i] = 1;
	}
		MPI_Cart_create(MPI_COMM_WORLD, ndims, dim_size, periods, reorder, &New_Comm);
	//	MPI_Cartdim_get(New_Comm, &count_dim);
		//MPI_Cart_get(New_Comm, ndims, dim_size, periods, coord);
	for (int i = 0; i < ndims; i++) {
		if (periods[i] != 1) {
			return false;
		}
	}
	//bool TestSum = TestCube(New_Comm, ndims, SizeNodeHyperCube, local_data);
	if (ProcRank == 0) {
		sendr(ProcSize, local_data, neproc, New_Comm);
	}
	MPI_Comm_free(&New_Comm);
	MPI_Finalize();
	delete[] dim_size;
	delete[] periods;
	return 0;
}