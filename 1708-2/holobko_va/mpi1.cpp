#include "pch.h"
#include <time.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <mpi.h>
#include <vector>
#include <time.h>
#include <algorithm>
using namespace std;

int MinElem(vector<int> Myvector) {
	vector<int>::iterator result = min_element(Myvector.begin(), Myvector.end());
	int temp = *result;
	return temp;
}

vector<int> GenerateVector(int Size, int MaxVal) {
	vector<int> Myvector(Size);
	cout << "\nVector: ";
		for (int i = 0; i < Size; i++)
			Myvector[i]=(rand()%MaxVal + 1);
		for (int n : Myvector)
			cout << n << "\t";
	return Myvector;
}

int main(int argc, char** argv) {
	srand(time(NULL));
	double TimeStartWork, TimeEndWork;
	int Size, MaxVal,*Data, Rank, ProcNum;
	vector<int> Myvector;
	cout << "Search Min Element Vector\nSize Vector: ";
	cin >> Size;
	cout << "Max Value At Elements Vector: ";
	cin >> MaxVal;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	int PartSize = Size/ProcNum;
	int *Part = new int[PartSize];
	Data = new int[Size];
	MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
		if (Rank == 0) {
			int Min;
			Myvector = GenerateVector(Size, MaxVal);
			TimeStartWork = MPI_Wtime();
			Min = MinElem(Myvector);
			TimeEndWork = MPI_Wtime();
			cout << endl << "Min In Single = " << Min << "\nTime In Single = " << TimeStartWork - TimeEndWork << " sec" << endl;
			MPI_Scatter(Data, PartSize, MPI_INT, Part, PartSize, MPI_INT, 0, MPI_COMM_WORLD);
		}
		else 
			MPI_Scatter(nullptr, PartSize, MPI_INT, Part, PartSize, MPI_INT, 0, MPI_COMM_WORLD);
	TimeStartWork = MPI_Wtime();
	int Min = MinElem(Myvector);
		if (Rank == 0) {
			int result;
			MPI_Reduce(&Min, &result, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
				if (Size % ProcNum != 0) {
					for (int i = PartSize * ProcNum; i < Size; i++)	{
						if (result > Data[i])
							result = Data[i];
				}
			}
			TimeEndWork = MPI_Wtime();
			cout << "Min In Parallel =  " << result << "\nTime In Parallel = " << std::fixed << TimeStartWork - TimeEndWork << " sec" << endl;
		}
		else
			MPI_Reduce(&Min, nullptr, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}