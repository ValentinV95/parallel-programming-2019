#include "pch.h"
#include <time.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <mpi.h>
using namespace std;

// Количество элементов в векторе
#define ARR_SIZE 100000
#define PRINT_SIZE 1000

// Заполняем вектор случайными числами
void fillArray(int *m, int size)
{
	srand(1);
	if (size <= PRINT_SIZE) {
		printf("\nArray:");
		for (int i = 0; i < size; i++)
		{
			m[i] = rand() % 20 - 10;
			printf(" %d", m[i]);
		}
	}
	else {
		for (int i = 0; i < size; i++)
		{
			m[i] = rand() % 20 - 10;
		}
	}
	m[size] = 0;
}

int main(int argc, char* argv[]) {

	//Инициализация
	int ProcRank, ProcNum, TotalSum = 0, ProcSum = 0, size = ARR_SIZE, SeqSum = 0;
	int *m = new int[size + 1];
	double start, finish, seqtime, partime;
	MPI_Status Status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

	if (ProcRank == 0) {
		fillArray(m, size);
		start = MPI_Wtime();
		for (int i = 0; i < size; i++) {
			if (m[i] < 0 && m[i + 1]>0 || m[i] > 0 && m[i + 1] < 0)
				SeqSum++;
		}
		finish = MPI_Wtime();
		seqtime = finish - start;
		printf("Array size is %d\n", size);
		printf("\n\nThe sequential result is %3d\n", SeqSum);
		printf("Time elapsed %1.10f\n", seqtime);
	}


	// Синхронизируем все потоки
	MPI_Barrier(MPI_COMM_WORLD);

	if (ProcRank == 0)	start = MPI_Wtime();
	//Рассылаем данные, задаем границы
	int part = size / ProcNum;
	if (ProcRank == 0) {
		int RecvNum;

		for (int i = 1; i < ProcNum - 1; i++)
			MPI_Send(m + part * i, part + 1, MPI_INT, i, 1, MPI_COMM_WORLD);

		if (ProcNum > 1) {
			int lastPart = size - part * (ProcNum - 1);
			MPI_Send(m + part * (ProcNum - 1), lastPart + 1, MPI_INT, ProcNum - 1, 1, MPI_COMM_WORLD);
		}

		for (int i = 0; i < part; i++)
			if (m[i] < 0 && m[i + 1]>0 || m[i] > 0 && m[i + 1] < 0)
				TotalSum++;

		for (int i = 1; i < ProcNum; i++) {
			MPI_Recv(&RecvNum, 1, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &Status);
			TotalSum += RecvNum;
		}
	}
	else {

		if (ProcRank == ProcNum - 1)
			part = size - part * (ProcNum - 1);

		int *pm = new int[part + 1];
		MPI_Recv(pm, part + 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &Status);

		for (int i = 0; i < part; i++) {
			if (pm[i] < 0 && pm[i + 1]>0 || pm[i] > 0 && pm[i + 1] < 0)
				ProcSum++;
		}

		delete[] pm;

		MPI_Send(&ProcSum, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if (ProcRank == 0) {
		finish = MPI_Wtime();
		partime = finish - start;
		printf("\nThe parallel result is %d\n", TotalSum);
		printf("Time elapsed %1.10f\n", partime);
		printf("\nThe acceleration is %1.6f\n\n", seqtime / partime);
	}
	delete[] m;
	MPI_Finalize();

}
