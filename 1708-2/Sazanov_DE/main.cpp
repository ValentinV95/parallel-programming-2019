//#include <iostream>
//#include <ctime>
//#include <mpi.h>
//
//using namespace std;
//
//
//
//int main(int argc, char** argv)
//{
//	int n, m, matmax, *mmax, max(0), size;
//	int ProcNum, Rank;
//	double *mat;
//	double TimeStartWork, TimeEndWork;
//	srand(time(0));
//
//	cout << "Enter matrix n - "; cin >> n;
//	cout << "Enter matrix m - "; cin >> m;
//	cout << "Enter matrix maxmat - "; cin >> matmax;
//	mat = new double[n*m];
//	mmax = new int[m];
//	size = n * m;
//
//	//MPI_Init(&argc, &argv);
//	//MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
//	//MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
//
//	//int PartSize = size / ProcNum;
//	//int Resiadue = size % ProcNum;
//
//	for (int i = 0; i < n; i++)
//		for (int j = 0; j < m; j++)
//			*(mat + i * m + j) = rand() % (matmax + 1);
//	
//	for (int i = 0; i < n; i++)
//	{
//		for (int j = 0; j < m; j++)
//			cout << *(mat + i * m + j) << "\t";
//		cout << endl;
//	}
//
//	for (int i = 0; i < m; i++)
//	{
//		for (int j = 0; j < n; j++)
//			if (*(mat + j * m + i) > max)
//			{
//				max = *(mat + j * m + i);
//				mmax[i] = *(mat + j * m + i);
//			}
//		max = 0;
//	}
//	cout << endl;
//	for (int i = 0; i < m; i++)
//		cout << mmax[i]<< "\t";
//	cin >> n;
//	system("pause");
//}