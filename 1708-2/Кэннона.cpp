#include "stdafx.h"
#include <iostream>
#include "omp.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"


using namespace std;
using namespace tbb;


class CannonAlgorithm
{
	const double *AM, *BM;
	double *const CM;
	int const N;
	int const B;

public:
	CannonAlgorithm(double *AM, double *BM, double *CM, int N, int B) :
		AM(AM), BM(BM), CM(CM), N(N), B(B) {}

	void operator()(const blocked_range<int>& r) const
	{
		int begin = r.begin(), end = r.end();
		for (int block = begin; block != end; block++)
		{
			int ib = block / B;
			int jb = block - ib * B;

			int jj = (ib + jb) % B;

			for (int k = 0; k < B; k++)
			{
				for (int mi = ib * (N / B); mi < (ib + 1)*(N / B); mi++)
				for (int mj = jb * (N / B); mj < (jb + 1)*(N / B); mj++)
				{
					double SUM = 0.0;
					for (int mk = jj * (N / B); mk < (jj + 1)*(N / B); mk++)
						SUM += AM[mi*N + mk] * BM[mk*N + mj];
					CM[mi*N + mj] += SUM;
				}

				jj = (jj + 1) % B;
			}
		}
	}
};

double CannonAlgorithm_Seq(double * AM, double * BM, double * CM, int N, int B)
{
	double start = 0, finish = 0;

	start = omp_get_wtime();
	for (int block = 0; block < B*B; block++)
	{
		int ib = block / B;
		int jb = block - ib * B;

		int jj = (ib + jb) % B;

		for (int k = 0; k < B; k++)
		{
			for (int mi = ib * (N / B); mi < (ib + 1)*(N / B); mi++)
			for (int mj = jb * (N / B); mj < (jb + 1)*(N / B); mj++)
			{
				double SUM = 0.0;
				for (int mk = jj * (N / B); mk < (jj + 1)*(N / B); mk++)
					SUM += AM[mi*N + mk] * BM[mk*N + mj];
				CM[mi*N + mj] += SUM;
			}

			jj = (jj + 1) % B;
		}
	}
	finish = omp_get_wtime();

	return (finish - start);
}

double CannonAlgorithm_OMP(double * AM, double * BM, double * CM, int N, int B)
{
	double start = 0, finish = 0;

	start = omp_get_wtime();
#pragma omp parallel for
	for (int block = 0; block < B*B; block++)
	{
		int ib = block / B;
		int jb = block - ib * B;

		int jj = (ib + jb) % B;

		for (int k = 0; k < B; k++)
		{
			for (int mi = ib * (N / B); mi < (ib + 1)*(N / B); mi++)
			for (int mj = jb * (N / B); mj < (jb + 1)*(N / B); mj++)
			{
				double SUM = 0.0;
				for (int mk = jj * (N / B); mk < (jj + 1)*(N / B); mk++)
					SUM += AM[mi*N + mk] * BM[mk*N + mj];
				CM[mi*N + mj] += SUM;
			}

			jj = (jj + 1) % B;
		}
	}
	finish = omp_get_wtime();

	return (finish - start);
}

double CannonAlgorithm_TBB(double * AM, double * BM, double * CM, int N, int B, int grainSize)
{
	task_scheduler_init init;
	double start = 0, finish = 0;

	start = omp_get_wtime();
	parallel_for(blocked_range<int>(0, B*B, grainSize), CannonAlgorithm(AM, BM, CM, N, B));
	finish = omp_get_wtime();

	return (finish - start);
}


void main()
{
	int B = 40;			// grid size B*B
	int b = 50;			// block size b*b
	int N = B * b;		// matrix size: N*N

	double *AM = new double[N*N];
	double *BM = new double[N*N];
	double *CM = new double[N*N];

	for (int i = 0; i < N*N; i++)
	{
		AM[i] = rand() % 5 + 1;
		BM[i] = rand() % 5 + 1;
		CM[i] = 0.0;
	}

	double TIME = 0.0;
	cout << "SIZE: " << N << endl;

	TIME = CannonAlgorithm_Seq(AM, BM, CM, N, B);
	cout << "TIME Seq: " << TIME << endl;

	TIME = CannonAlgorithm_OMP(AM, BM, CM, N, B);
	cout << "TIME OMP: " << TIME << endl;

	TIME = CannonAlgorithm_TBB(AM, BM, CM, N, B, 1);
	cout << "TIME TBB: " << TIME << endl;

	bool visual = false;
	if (visual)
	{
		cout << "A:" << endl;
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				cout << AM[i*N + j] << "\t";
			}
			cout << endl;
		}
		cout << endl;
		cout << "B:" << endl;
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				cout << BM[i*N + j] << "\t";
			}
			cout << endl;
		}
		cout << endl;
		cout << "C:" << endl;
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				cout << CM[i*N + j] << "\t";
			}
			cout << endl;
		}
	}

	cin.get();
}