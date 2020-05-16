// Copyright 2020 Blagin Invan
#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
using namespace std;

void Print_Matrix(double* matrix, int N) {
	for (int i = 0; i < N * N; i += N) 
	{
		for (int j = 0; j < N; j++)
		{
			cout << matrix[i + j] << " ";
		}			
		cout << std::endl;
	}
	cout << std::endl;
}

void Multtiplication(double* A, double* B, double* C, int blockSize, int N) {
	for (int i = 0; i < blockSize; ++i)
		for (int j = 0; j < blockSize; ++j)
			for (int k = 0; k < blockSize; ++k) 
			{
				C[i * N + j] += A[i * N + k] * B[k * N + j];
			}
}

void EXECUTE(double* A, double* B, double* C, int Size_M, int Proc_N) {
	int Size_B = Size_M / Proc_N;
	for (int i = 0; i < Proc_N; ++i) {
		for (int j = 0; j < Proc_N; ++j) {
			for (int k = 0; k < Proc_N; ++k) {
				Multtiplication(&A[(i * Size_M + (j + i + k) % Proc_N) * Size_B], &B[(((i + j + k) % Proc_N) * Size_M + j) * Size_B],
					&C[(i * Size_M + j) * Size_B], Size_B, Size_M);
			}
		}
	}
}

void EXECUTE_PARALLEL(double* A, double* B, double* C, int Proc_N, int Size_M) {
	omp_set_num_threads(Proc_N);
	int Size_G = static_cast <int>(sqrt(Proc_N));
	int Size_B = Size_M / Size_G;
#pragma omp parallel
	{
		int Curr = omp_get_thread_num();
		int Row_I = Curr / Size_G;
		int Col_I = Curr % Size_G;
		for (int iter = 0; iter < Size_G; iter++) {
			for (int i = Row_I * Size_B; i < (Row_I + 1) * Size_B; i++)
				for (int j = Col_I * Size_B; j < (Col_I + 1) * Size_B; j++)
					for (int k = iter * Size_B; k < (iter + 1) * Size_B; k++) {
						C[i * Size_M + j] += A[i * Size_M + k] * B[k * Size_M + j];
					}
		}
	}
}

int main(int argc, char** argv) {
	int Size_Matrix = atoi(argv[1]), Proc_N = atoi(argv[2]), Check = 0;
	if (Size_Matrix % Proc_N != 0)
	{
		cout << "Size fo matrices must divided by value of threads!" << endl;
		return 0;
	}
	double* A = new double[Size_Matrix * Size_Matrix], * B = new double[Size_Matrix * Size_Matrix], * C = new double[Size_Matrix * Size_Matrix], * RSA = new double[Size_Matrix * Size_Matrix], * RSB = new double[Size_Matrix * Size_Matrix], * RSC = new double[Size_Matrix * Size_Matrix];
	double Time_parallel = 0, Time_single = 0, Time_Easy = 0;
	cout << "\n\n\tTotal threads = " << omp_get_max_threads() << "\n\tTotal processes = " << Proc_N << endl;
	
	for (int i = 0; i < Size_Matrix * Size_Matrix; ++i)		// clear matrix
	{
		C[i] = 0;
		RSC[i] = 0;
		RSB[i] = 0;
		RSA[i] = 0;
	}
	
	for (int i = 0; i < Size_Matrix * Size_Matrix; ++i)
	{
		A[i] = (rand() % 10000) / 1000.0f;
		B[i] = (rand() % 10000) / 1000.0f;
	}

	if (Size_Matrix < 5)
	{
		cout << "\n\n\tMatrice A:\n\n";
		Print_Matrix(A, Size_Matrix);

		cout << "\n\n\tMatrice B:\n\n";
		Print_Matrix(B, Size_Matrix);
	}

	Time_Easy = omp_get_wtime();
	for (int i = 0; i < Size_Matrix; ++i)
		for (int j = 0; j < Size_Matrix; ++j)
			for (int k = 0; k < Size_Matrix; ++k) {
				RSA[i * Size_Matrix + j] += A[i * Size_Matrix + k] * B[k * Size_Matrix + j];
			}
	Time_Easy = omp_get_wtime() - Time_Easy;

	Time_single = omp_get_wtime();
	EXECUTE(A, B, C, Size_Matrix, Proc_N);
	Time_single = omp_get_wtime() - Time_single;

	Time_parallel = omp_get_wtime();
	EXECUTE_PARALLEL(A, B, RSB, Proc_N, Size_Matrix);
	Time_parallel = omp_get_wtime() - Time_parallel;


	if (Size_Matrix < 5) 
	{		
		cout << "\n\n\tMatrice C:\n\n";
		Print_Matrix(C, Size_Matrix);	
	}

	for (int i = 0; i < Size_Matrix; i++)
		for (int j = 0; j < Size_Matrix; j++) {
			if (fabs(RSB[i * Size_Matrix + j] - C[i * Size_Matrix + j]) < 0.1)
				Check++;
			else
				Check = 0;
		}

	
	if (Check == 0)
		cout << "Bad result. Try another values of size & process" << endl;
	else
	{
		cout << "\n\nTotal time simple version =  " << Time_Easy << " seconds." << "\n\n";
		cout << "Total time sequental version =  " << Time_single << " seconds." << "\n\n";
		cout << "Total time parallel version =  " << Time_parallel << " seconds." << "\n\n";
		cout << "Total PROFIT = " << Time_Easy / Time_parallel << "\n\n";
	}	
	return 0;
}
