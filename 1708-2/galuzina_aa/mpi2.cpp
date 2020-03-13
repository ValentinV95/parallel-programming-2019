#include <stdio.h>
#include <iostream>
#include "mpi.h"
using namespace std;

int main(int argc, char* argv[])
{
	int Rank, ProcN, SizeM, Operations, Index_Counter = 10; //Rank - текущий проц., ProcN - количество процессов, SizeM - размер массива, Operations - количество связок отправитель-получатель, Index_Counter - количество элементов массива на 1 проц.
	double WTimeStart, WTimeEnd;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcN);
	MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
	if (ProcN % 2 == 0 && ProcN != 2)
	{
		Operations = (ProcN / 2);
		SizeM = Operations * 10;
		int* Massive = new int[SizeM]; // по 10 элементов на каждую отправку
		if (Rank == 0)
		{
			cout << "\n\t\tManufacturers and consumers.\n\t\tGaluzina A. 381708-2.\n\n\tOperations send-receive: " << Operations << "\n\tSize of massive: " << SizeM;
			for (int i = 0; i < SizeM; i++)
			{
				Massive[i] = i + 1;
			}
			cout << "\n\nSource massive: ";
			for (int i = 0; i < SizeM; i++)
			{
				cout << Massive[i] << "|";
			}
			for (int i = 1; i <= Operations - 1; i++) // Рассылка от рут-процесса всем процессам-отправителям, чтобы в дальнейшем процессы-отправители отправили данные процессам-получателям, -1, т.к. рут-процесс изначально тоже является процессом-отправителем и в нём уже есть данные, ему отправлять не нужно
			{
				MPI_Send(&Massive[Index_Counter], 10, MPI_INT, i, 500, MPI_COMM_WORLD);
				Index_Counter += 10;
			}
		}
		if (Operations == 2)
		{
			if (Rank == 1) // получение данных процессами-отправителями (кроме рута, в нём изначально есть данные, он тоже процесс-отправитель)
			{
				MPI_Recv(&Massive[0], 10, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
			if (Rank == 0 || Rank == 1)
			{
				MPI_Send(&Massive[0], 10, MPI_INT, Rank + 2, 500, MPI_COMM_WORLD);
			}
		}
		if (Operations == 3)
		{
			if (Rank == 1 || Rank == 2)
			{
				MPI_Recv(&Massive[0], 10, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
			if (Rank == 0 || Rank == 1 || Rank == 2)
			{
				MPI_Send(&Massive[0], 10, MPI_INT, Rank + 3, 500, MPI_COMM_WORLD);
			}
		}
		if (Operations == 4)
		{
			if (Rank == 1 || Rank == 2 || Rank == 3)
			{
				MPI_Recv(&Massive[0], 10, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
			if (Rank == 0 || Rank == 1 || Rank == 2 || Rank == 3)
			{
				MPI_Send(&Massive[0], 10, MPI_INT, Rank + 4, 500, MPI_COMM_WORLD);
			}
		}
		// Рассылка от процессов-отправителей к процессам-получателям



		if (Operations == 2)
		{
			if (Rank == 2 || Rank == 3) // получение данных процессами-получателями
			{
				MPI_Recv(&Massive[0], 10, MPI_INT, Rank - 2, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
		}
		if (Operations == 3)
		{
			if (Rank == 3 || Rank == 4 || Rank == 5)
			{
				MPI_Recv(&Massive[0], 10, MPI_INT, Rank - 3, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}

		}
		if (Operations == 4)
		{
			if (Rank == 4 || Rank == 5 || Rank == 6 || Rank == 7)
			{
				MPI_Recv(&Massive[0], 10, MPI_INT, Rank - 4, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
		}

		WTimeStart = MPI_Wtime();

		cout << "\nMessage from procces #" << Rank << ":";
		cout << '|';
		for (int i = 0; i < 10; i++)
		{
			cout << Massive[i] << '|';
		}


		if (Rank == 0)
		{
			WTimeEnd = MPI_Wtime();
			cout << "\n\nComputation time = " << WTimeEnd - WTimeStart << " seconds\n\n";
		}
		MPI_Finalize();
		return 0;
	}
	else
	{
		cout << "Total process must be 4, 6, or 8!";
		MPI_Finalize();
		return 0;
	}
}
