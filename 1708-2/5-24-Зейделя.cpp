#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <omp.h>
#include <time.h>

using namespace std;

#define NUM_OF_THREADS 4
#define N 20

double *gaus(double **a){
	double *x = (double*)malloc(sizeof(double)*N);
	for (int i = 0; i < N; i++) {
		x[i] = a[i][N];
	}
	double m;
	for (int k = 1; k < N; k++) {
		for (int j = k; j < N; j++) {
			m = a[j][k - 1] / a[k - 1][k - 1];
			for (int i = 0; i < N + 1; i++) {
				a[j][i] = a[j][i] - m * a[k - 1][i];
			}
			x[j] = x[j] - m * x[k - 1];
		}
	}

	for (int i = N - 1; i >= 0; i--) {
		for (int j = i + 1; j < N; j++) {
			x[i] -= a[i][j] * x[j];
		}
		x[i] = x[i] / a[i][i];
	}
	return x;
}

double* gaus_mp(double **a){

	double *x = (double*)malloc(sizeof(double)*N);
	for (int i = 0; i < N; i++) {
		x[i] = a[i][N];
	}
	double m;
	int k, j, i;
	for (k = 1; k < N; k++) {
#pragma omp parallel for private(m,i,j)
		for (j = k; j < N; j++) {
			m = a[j][k - 1] / a[k - 1][k - 1];
			for (i = 0; i < N + 1; i++) {
				a[j][i] = a[j][i] - m * a[k - 1][i];
			}
			x[j] -= m * x[k - 1];
		}
	}

#pragma omp for private(j) ordered 
	for (i = N - 1; i >= 0; i--) {
		for (j = i + 1; j < N; j++){
			x[i] -= a[i][j] * x[j];
		}
		x[i] = x[i] / a[i][i];
	}
	return x;
}

int main()
{
	//memory block
	double **a, **b;
	a = (double**)malloc(sizeof(double*)*N);
	b = (double**)malloc(sizeof(double*)*N);
	for (int i = 0; i < N + 1; i++){
		a[i] = (double*)malloc(sizeof(double)*(N + 1));
		b[i] = (double*)malloc(sizeof(double)*(N + 1));
	}
	//randomize matrix
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N + 1; j++) {
			a[i][j] = rand() % (N*N);
			b[i][j] = a[i][j];
		}
	}
	//output matrix
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N + 1; j++) {
			cout << a[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	double start_time;
	start_time = omp_get_wtime();
	double *x = gaus(a);
	double line = omp_get_wtime() - start_time;

	cout << "result line: ";
	for (int i = 0; i < N; i++)
		cout << x[i] << " ";
	cout << endl;
	cout << "line time " << line << endl << endl;

	start_time = omp_get_wtime();
	x = gaus_mp(b);
	double multi = omp_get_wtime() - start_time;

	cout << "result multi: ";
	for (int i = 0; i < N; i++)
		cout << x[i] << " ";
	cout << endl;
	cout << "multi time " << multi << endl;

	getchar();
	return 0;
}

