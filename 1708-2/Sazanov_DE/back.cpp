#include <iostream>
#include <ctime>
#include <vector>
#include <mpi.h>

using namespace std;

void mGen(int**& m, int mSize, int mMax)
{
	for (int i = 0; i < mSize; i++)
	{
		for (int j = 0; j < mSize; j++)
			m[i][j] = rand() % mMax;
	}
}
void MatToMas(int**& m, int*& mMas, int mSize)
{
	for (int i = 0; i < mSize; i++)
		for (int j = 0; j < mSize; j++)
			mMas[i*mSize + j] = m[j][i];
}
void mShow(int**&m, int mSize)
{
	for (int i = 0; i < mSize; i++)
	{
		for (int j = 0; j < mSize; j++)
			cout << m[i][j] << "\t";
		cout << "\n\n";
	}
}
void mMinShow(int *&mMin, int mSize)
{
	for (int i = 0; i < mSize; i++)
		cout << mMin[i] << "\t";
	cout << "\n\n";
}
void mMasShow(int *&mMas, int mSize)
{
	for (int i = 0; i < mSize*mSize; i++)
		cout << mMas[i];
	cout << "\n\n";
}
void mMinCalc(int **&m, int *&mMin, int check, int mSize)
{
	for (int i = 0; i < mSize; i++)
	{
		check = 0;
		for (int j = 0; j < mSize; j++)
		{
			if (m[j][i] > check)
			{
				check = m[j][i];
				mMin[i] = check;
			}
		}
	}
}

int  main(int argc, char** argv)
{
	int mSize, mMax, check, eMax; 
	int ProcNum, ProcRank, pSize;
	int *mMin, *mMas;
	double sTime(0), eTime(0);

	srand(time(0));

	//cout << "Enter mSize: "; cin >> mSize;
	//cout << "Enter mMax: "; cin >> mMax;
	mSize = 100000;
	mMax = 1024;
	int** m = new int*[mSize];
	mMas = new int[mSize*mSize];
	mMin = new int[mSize];

	for (int i = 0; i < mSize; i++)
		m[i] = new int[mSize];

	mGen(m, mSize, mMax);
	//mShow(m, mSize);
	//mMinCalc(m, mMin, check, mSize);

	//mMinShow(mMin, mSize);
	MatToMas(m, mMas, mSize);
	//mMasShow(mMas, mSize);

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

	pSize = mSize / ProcNum;
	if (mSize % ProcNum != 0)
		pSize++;

	if (ProcRank == 0)
		sTime = MPI_Wtime();

	MPI_Bcast(mMas, mSize*mSize, MPI_INT, 0, MPI_COMM_WORLD);

	for (int i = pSize * ProcRank; i < pSize*(ProcRank + 1); i++)
	{
		eMax = 0;
		for (int j = 0; j < mSize; j++)
			if (mMas[i + j * mSize] > eMax)
				eMax = mMas[i + j * mSize];
		if (ProcRank == 0)
			mMin[i] = eMax;
		else
			MPI_Send(&eMax, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	if (ProcRank == 0)
	{
		for(int i = 1; i < ProcNum; i++)
			for (int j = 0; j < pSize; j++)
			{
				MPI_Status status;
				MPI_Recv(&eMax, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
				mMin[i*pSize + j] = mMax;
			}
		eTime = MPI_Wtime();
		cout << "0Time: " << eTime - sTime << endl;
		//for (int i = 0; i < mSize; i++)
		//	cout << mMin[i] << "\t";
	}
	MPI_Finalize();

	return 0;
}