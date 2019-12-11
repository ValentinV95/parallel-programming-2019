#include "mpi.h"
#include <vector>
#include <string>
#include <random>
#include <stdlib.h>
#include <ctime>
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <ctime>
#include <cmath>
#include <string>

using namespace std;

int getPositive_elem(std::vector<int> vector, int count_size_vector) {
	if (count_size_vector < 2) {
		return 0;
	}
	int negative_elem = 0;
	for (int c = 0; c < count_size_vector; c++) {
		if (vector[c] == -1) {
			negative_elem++;
		}
	}
	return vector.size() - negative_elem;
}
int* Create_dinamic_massiv_from_vector(std::vector<int> vec) {
	int vec_size = vec.size();
	int* mas = new int[vec_size];
	for (int i = 0; i < vec_size; i++) {
		mas[i] = vec[i];
	}
	return mas;
}

int Consumer(int *buffer, int buffer_size, int rank_proc, int* resurce) {
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (size == 1 || rank_proc == 0) {
		for (int i = 0; i < buffer_size; i++) {
			if (buffer[i] != -1) {
				*resurce = buffer[i];
				buffer[i] = -1;
				break;
			}
		}
	}
	else {
		if (rank == 0) {
			int temp_resurs;
			for (int i = 0; i < buffer_size; i++) {
				if (buffer[i] != -1) {
					temp_resurs = buffer[i];
					buffer[i] = -1;
					break;
				}
			}
			MPI_Send(&temp_resurs, 1, MPI_INT, rank_proc, 0, MPI_COMM_WORLD);
		}
		else {
			if (rank == rank_proc) {
				int temp_resurs;
				MPI_Recv(&temp_resurs, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				*resurce = temp_resurs;
			}
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);
	return 0;
}
int Producer(int *buffer, int buffer_size, int rank_proc, int resurce) {
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (size == 1 || rank_proc == 0) {
		for (int i = 0; i < buffer_size; i++) {
			if (buffer[i] == -1) {
				buffer[i] = resurce;
				break;
			}
		}
	}
	else {
		if (rank == 0) {
			int resurce_for_bufer;
			MPI_Recv(&resurce_for_bufer, 1, MPI_INT, rank_proc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for (int i = 0; i < buffer_size; i++) {
				if (buffer[i] == -1) {
					buffer[i] = resurce;
					break;
				}
			}
		}
		else {
			if (rank == rank_proc) {
				MPI_Send(&resurce, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			}
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);
	return 0;
}
int main(int argc, char** argv)
{
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int kol_elem_in_buffer = 100;
	int *buffer = new int[kol_elem_in_buffer];
	for (int i = 0; i < kol_elem_in_buffer; i++) {
		buffer[i] = -1;
	}
	int kol_resursov = atoi(argv[1]);
	if (size == 1) {
		for (int i = 0; i < kol_resursov; i++) {
			Producer(buffer, kol_elem_in_buffer, rank, i);
		}
		std::vector<int> resurce_consume1(kol_resursov, -1);
		int *resurce_consume;
		resurce_consume = Create_dinamic_massiv_from_vector(resurce_consume1);
		for (int i = 0; i < kol_resursov; i++) {
			Consumer(buffer, kol_elem_in_buffer, rank, &resurce_consume[i]);
			cout << resurce_consume[i] << "\n";
		}
	}
	else {
		for (int i = 0; i < kol_resursov; i++) {
			Producer(buffer, kol_elem_in_buffer, 1, i);
		}
		std::vector<int> resurce_consume1(kol_resursov, -1);
		int *resurce_consume;
		resurce_consume = Create_dinamic_massiv_from_vector(resurce_consume1);
		for (int i = 0; i < kol_resursov; i++) {
			Consumer(buffer, kol_elem_in_buffer, 1, &resurce_consume[i]);
		}
		if (rank == 1) {
			for (int i = 0; i < kol_resursov; i++) {
				cout << resurce_consume[i] << "\n";
			}
		}
	}
	MPI_Finalize();
	return 0;
}