#include <stdio.h>
#include <iostream>
#include "mpi.h"
using namespace std;

int main(int argc, char* argv[3])
{
	int Root = atoi(argv[1]), Proc_N, Size_M = atoi(argv[2]), Part_Size, Additional_Pack, Rank , Index_Counter = 0;
	double W_Time_Start, W_Time_End;
	int* Massive = new int[Size_M];	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &Proc_N);
	Part_Size = (Size_M / (Proc_N - 1));
	Additional_Pack = Size_M / Proc_N;
	MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
	if (Rank == Root)
	{
		cout << "\n\t\tMPI_Scater realization.\n\t\tTimofeev E.V. 381708-2.\n\n";
		cout << "\tRoot process = " << Root << "\n\tAmount of process = " << Proc_N << "\n\tAverage sent package size = " << Part_Size << "\n\tSize of massive = " << Size_M << endl;
		for (int i = 0; i < Size_M; i++)
		{
			Massive[i] = i + 1;
		}
		cout << "\nInput massive: ";
		for (int i = 0; i < Size_M; i++)
		{
			cout << Massive[i]<<'|';
		}
		W_Time_Start = MPI_Wtime();		
		for (int i = 0; i < Proc_N; i++)
		{ 
			if (i!=Root)
			{
				MPI_Send(&Massive[Index_Counter], Part_Size, MPI_INT, i, 500, MPI_COMM_WORLD);
				Index_Counter += Part_Size;
			}			
		}				
	}
	if(Rank!=Root)
	{
		MPI_Recv(&Massive[0], Part_Size, MPI_INT, Root, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for (int i = 0; i < Part_Size; i++)
		{
			cout << Massive[i] << '|';
		}
	}
	if (Rank == Root)
	{		
		W_Time_End = MPI_Wtime();
		cout << "\n\nComputation time = " << W_Time_End - W_Time_Start << " seconds\n" << endl;
		for (int i = Index_Counter; i < Size_M; i++)
		{
			cout << Massive[i] << '|';
		}
	}
	MPI_Finalize();
	return 0;
}

