#include <stdio.h>
#include <iostream>
#include "mpi.h"
using namespace std;


int main(int argc, char* argv[])
{
	int Rank, ProcN, Size_V = 27, Max_Val, Proc_result = 0; // static size of vectors
	double WTimeStart, WTimeEnd;
	long Result;
	int* VectorX = new int[Size_V];
	int* VectorY = new int[Size_V];
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcN);
	MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
	if (Rank == 0)
	{
		cout << "\n\t\tScalar product of two vectors.\n\t\tTimofeev E.V. 381708-2.\n\n";
		cout << "\nEnter the max value of vectors:\n";
		cin >> Max_Val;
		WTimeStart = MPI_Wtime();
		for (int i = 0; i < Size_V; i++)
		{
			VectorX[i] = (rand() % Max_Val + 1);
			VectorY[i] = (rand() % Max_Val + 1);
		}
		cout << "Vector X: ";
		for (int i = 0; i < Size_V; i++)
		{
			cout << VectorX[i] << "|";
		}
		cout << "\nVector Y: ";
		for (int i = 0; i < Size_V; i++)
		{
			cout << VectorY[i] << "|";
		}
	}
	MPI_Bcast(VectorX, Size_V, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(VectorY, Size_V, MPI_INT, 0, MPI_COMM_WORLD);
	for (int i = Rank * Size_V / ProcN; i < (Rank + 1) * Size_V / ProcN; i++)
	{
		Proc_result  += VectorX[i] * VectorY[i];
	}
	MPI_Reduce(&Proc_result, &Result, 1, MPI_INT, MPI_SUM, 0,MPI_COMM_WORLD);	
	if (Rank == 0)
	{
		WTimeEnd = MPI_Wtime();
		cout << "\n\nResult = " << Result <<"\nComputation time = "<< WTimeEnd - WTimeStart<< " seconds\n\n";	
	}
	MPI_Finalize();
	return 0;
}


