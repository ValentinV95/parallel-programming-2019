#include <time.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <mpi.h>
#include <vector>
#include <time.h>
#include <stdexcept>
#include <algorithm>
using namespace std;

int main(int argc, char** argv) {
	int ProcNum, Rank, Size = atoi(argv[1]), MaxVal = atoi(argv[2]);
	double TimeStartWork, TimeEndWork;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
	int* Vector = new int[Size];

	if (Rank == 0) {
		for (int i = 0; i < Size; i++)
		{
			Vector[i] = (rand() % MaxVal + 1);			
		}
		TimeStartWork = MPI_Wtime();
		int Max = Vector[0];
		for (int i = 1; i < Size; i++) 
		{
			if (Vector[i] > Max) 
			{
				Max = Vector[i];
			}
		}
		TimeEndWork = MPI_Wtime();
		cout << endl << "Max In Single = " << Max << "\nTime In Single = " << TimeEndWork - TimeStartWork << " sec\n\n\n" << endl;		
	}
	TimeStartWork = MPI_Wtime();
	MPI_Bcast(Vector, Size, MPI_INT, 0, MPI_COMM_WORLD);
	int  Result = 0, ProcResult = 0;
	ProcResult = Vector[0];
	for (int i = 1; i < Size; i++) 
	{
		if (Vector[i] > ProcResult) 
		{
			ProcResult = Vector[i];
		}
	}	
	MPI_Reduce(&ProcResult, &Result, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
	if (Rank == 0) {

		TimeEndWork = MPI_Wtime();
		cout << "Max In Parallel =  " << Result << "\nTime In Parallel = " << std::fixed << TimeEndWork - TimeStartWork << " sec" << endl;
	}
	MPI_Finalize();
	return 0;
}