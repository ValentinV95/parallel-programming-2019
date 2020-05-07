#include "pch.h"
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

int MinElem(vector<int> Myvector) {
	vector<int>::iterator result = min_element(Myvector.begin(), Myvector.end());
	int temp = *result;
	return temp;
}

vector<int> GenerateVector(int Size, int MaxVal) {
	vector<int> Myvector(Size);
	cout << "\nVector: ";
	for (int i = 0; i < Size; i++)
		Myvector[i] = (rand() % MaxVal + 1);
	//for (int n : Myvector)
	//	cout << n << "\t";
	return Myvector;
}

int main(int argc, char** argv) {
	srand(time(NULL));
	int ProcNum, Rank, Size = 1000000, MaxVal=1024;
	double TimeStartWork, TimeEndWork;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &Rank);	
	cout << "Search Min Element Vector\nSize Vector: ";
	//cin >> Size;
	cout << "Max Value At Elements Vector: ";
	//cin >> MaxVal;
	vector<int> Myvector(Size);
	Myvector = GenerateVector(Size, MaxVal);
	int PartSize = Size / ProcNum;
	int Residue = Size % ProcNum;
	vector<int> Part(PartSize);
	if (Rank == 0) {
		TimeStartWork = MPI_Wtime();
		int Min = MinElem(Myvector);
		TimeEndWork = MPI_Wtime();
		cout << endl << "Min In Single = " << Min << "\nTime In Single = " << TimeStartWork - TimeEndWork << " sec" << endl;
		for (int i = 1; i < ProcNum; i++)
			MPI_Send(&Myvector[Residue] + i * PartSize, PartSize, MPI_INT, i, 0, MPI_COMM_WORLD);
	}
	else if (Rank > 0) {
		TimeStartWork = MPI_Wtime();
		MPI_Status Status;
		vector<int> SubVec(PartSize);
		MPI_Recv(&SubVec[0], PartSize, MPI_INT, 0, 0, MPI_COMM_WORLD, &Status);
		int Min = *min_element(SubVec.begin(), SubVec.end());
		MPI_Send(&Min, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
	}
	if (Rank == 0) {
		int ResMin;
		MPI_Status  Status;
		int Result = *min_element(Myvector.begin(), Myvector.begin() + Residue);
		for (int i = 1; i < Size; i++) {
			MPI_Recv(&ResMin, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &Status);
			Result = std::min(Result, ResMin);
			TimeEndWork = MPI_Wtime();
			cout << "Min In Parallel =  " << Result << "\nTime In Parallel = " << std::fixed << TimeStartWork - TimeEndWork << " sec" << endl;
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}