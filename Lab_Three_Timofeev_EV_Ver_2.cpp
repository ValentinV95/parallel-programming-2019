#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <math.h>
#include <Windows.h>
#include <time.h>
#include <mpi.h>


#define ROOT 0
#define MAX_NZ 1000000
#define ZERO_IN_CRS 0.000001
#define EPS 0.01


int sizeP, rankP, size1;
size_t nzA, nzB;

struct crsMatrix
{
	int N;//размер матрицы N*N
	int NZ;//количество ненулевых элементов
	double* Value;//массив значений (размеров NZ)
	int* Col;//массив номеров столбцов (размером NZ)
	int* RowIndex;//массив индексов строк (размером N+1)
};

void InitializeMatrix(int N, int NZ, crsMatrix& mtx)
{
	mtx.N = N;
	mtx.NZ = NZ;
	mtx.Value = (double*)malloc(sizeof(double) * NZ);
	mtx.Col = (int*)malloc(sizeof(int) * NZ);
	mtx.RowIndex = (int*)malloc(sizeof(int) * (N + 1));

}
void DeleteMatrix(crsMatrix& mtx) {
	free(mtx.Value);
	free(mtx.Col);
	free(mtx.RowIndex);
}

void GenerateCRS(crsMatrix& mtx, int n, int cntInRow)
{
	int i, j, k, f, tmp, notNull, c;
	
	notNull = cntInRow * n;
	InitializeMatrix(n, notNull, mtx);
	for (i = 0; i < n; i++) {
		// Формируем номера столбцов в строке i 
		for (j = 0; j < cntInRow; j++)
		{
			do
			{
				mtx.Col[i * cntInRow + j] = rand() % n;
				f = 0;
				for (k = 0; k < j; k++)
					if (mtx.Col[i * cntInRow + j] == mtx.Col[i * cntInRow + k])
						f = 1;
			} while (f == 1);
		}
		// Сортируем номера столбцов в строке i 
		for (j = 0; j < cntInRow - 1; j++)
			for (k = 0; k < cntInRow - 1; k++)
				if (mtx.Col[i * cntInRow + k] > mtx.Col[i * cntInRow + k + 1])
				{
					tmp = mtx.Col[i * cntInRow + k];
					mtx.Col[i * cntInRow + k] = mtx.Col[i * cntInRow + k + 1];
					mtx.Col[i * cntInRow + k + 1] = tmp;
				}
	}
	for (i = 0; i < cntInRow * n; i++)
		mtx.Value[i] = rand() % 9 + 1;
	c = 0;
	for (i = 0; i <= n; i++)
	{
		mtx.RowIndex[i] = c;
		c += cntInRow;
	}
}

void PrintMatrix(int n, crsMatrix& mtx)
{
	int i;
	int k = mtx.NZ;
	int N = n + 1;

	printf("Matrix in CRS: \n");
	printf("\n Value: ");
	for (i = 0; i < k; i++) {
		printf(" |%.1f", mtx.Value[i]);
	}
	printf("\n Col:   ");
	for (i = 0; i < k; i++) {
		printf(" |%d", mtx.Col[i]);
	}
	printf("\n RowIndex: ");
	for (i = 0; i < N; i++) {
		printf(" |%d", mtx.RowIndex[i]);
	}
	printf("\n");
	fflush(stdout);
}

bool IsCompare(crsMatrix& A, crsMatrix& B) {
	int i, NZ = A.NZ;
	if ((A.NZ != B.NZ) || (A.N != B.N))
		return false;
	for (i = 0; i < NZ; i++)
		if ((A.Col[i] != B.Col[i]) || (fabs(A.Value[i] - B.Value[i]) > EPS))
			return false;
	return true;
}

void Transposition(crsMatrix& BT, crsMatrix& B) {
	//BT - траспонированная матрица В
	int tmp = 0, S = 0, IIndex = 0, RIndex = 0, i, j;
	double V = 0.0;
	InitializeMatrix(B.N, B.NZ, BT);

	memset(BT.RowIndex, 0, ((B.N) + 1) * sizeof(int));
	for (i = 0; i < B.NZ; i++)
		BT.RowIndex[B.Col[i] + 1]++;

	for (i = 1; i <= B.N; i++)
	{
		tmp = BT.RowIndex[i];
		BT.RowIndex[i] = S;
		S = S + tmp;
	}

	for (i = 0; i < B.N; i++)
	{

		int j1 = B.RowIndex[i];
		int j2 = B.RowIndex[i + 1];
		int Col = i; // Столбец в AT - строка в А
		for (j = j1; j < j2; j++)
		{
			V = B.Value[j]; // Значение
			RIndex = B.Col[j]; // Строка в AT
			IIndex = BT.RowIndex[RIndex + 1];
			BT.Value[IIndex] = V;
			BT.Col[IIndex] = Col;
			BT.RowIndex[RIndex + 1]++;
		}
	}
}

int SerialMultiplication(crsMatrix A, crsMatrix B, crsMatrix& C)
{
	int N = A.N, rowNZ, i, j, k, l;
	std::vector<int> columns;
	std::vector<double> values;
	std::vector<int> row_index;
	
	row_index.push_back(0);
	for (i = 0; i < N; i++)
	{
		rowNZ = 0;
		for (j = 0; j < N; j++)
		{
			double sum = 0;
			for (k = A.RowIndex[i]; k < A.RowIndex[i + 1]; k++)
			{

				for (l = B.RowIndex[j]; l < B.RowIndex[j + 1]; l++)
				{
					if (A.Col[k] == B.Col[l])
					{
						sum += A.Value[k] * B.Value[l];
						break;
					}
				}
			}
			if (fabs(sum) > ZERO_IN_CRS)
			{
				columns.push_back(j);
				values.push_back(sum);
				rowNZ++;
			}
		}
		row_index.push_back(rowNZ + row_index[i]);
	}
	InitializeMatrix(N, columns.size(), C);
	for (unsigned int j = 0; j < columns.size(); j++)
	{
		C.Col[j] = columns[j];
		C.Value[j] = values[j];
	}
	for (int i = 0; i <= N; i++)
		C.RowIndex[i] = row_index[i];
	return 0;
}
void MultiplicateOptim(crsMatrix A, crsMatrix B, crsMatrix& C)
{
	int N = A.N;
	int i, j, k;
	std::vector<int> columns;
	std::vector<double> values;
	std::vector<int> row_index;
	int NZ = 0;

	int* temp = new int[N];
	row_index.push_back(0);
	for (i = 0; i < N; i++)
	{
		// i-я строка матрицы A
		// Обнуляем массив указателей на элементы
		memset(temp, -1, N * sizeof(int));


		int ind1 = A.RowIndex[i], ind2 = A.RowIndex[i + 1];
		for (j = ind1; j < ind2; j++)
		{
			int col = A.Col[j];
			temp[col] = j;
		}

		for (j = 0; j < N; j++)
		{
			// j-я строка матрицы B
			double sum = 0;
			int ind3 = B.RowIndex[j], ind4 = B.RowIndex[j + 1];
			// Все ненулевые элементы строки j матрицы B
			for (k = ind3; k < ind4; k++)
			{
				int bcol = B.Col[k];
				int aind = temp[bcol];
				if (aind != -1)
					sum += A.Value[aind] * B.Value[k];
			}
			if (fabs(sum) > ZERO_IN_CRS)
			{
				columns.push_back(j);
				values.push_back(sum);
				NZ++;
			}
		}
		row_index.push_back(NZ);
	}
	InitializeMatrix(N, NZ, C);
	for (j = 0; j < NZ; j++)
	{
		C.Col[j] = columns[j];
		C.Value[j] = values[j];
	}
	for (i = 0; i <= N; i++)
		C.RowIndex[i] = row_index[i];
	delete[] temp;
}



void parallelMultiplicate(crsMatrix& result, crsMatrix A, crsMatrix B);


int main(int argc, char** argv) {

	double serialTime = 0.0, parallelTime = 0.0;
	double WTimeStart = 0.0, WTimeEnd = 0.0;
	int SizeM, NNZRow;
	crsMatrix BT, A, B, C, D;


	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &sizeP);
	MPI_Comm_rank(MPI_COMM_WORLD, &rankP);
	if (argc > 1) {
		SizeM = atoi(argv[1]);
		NNZRow = atoi(argv[2]);
	}

	if (rankP == ROOT) {
		GenerateCRS(A, SizeM, NNZRow);
		GenerateCRS(B, SizeM, NNZRow);

		nzA = A.NZ;
		nzB = B.NZ;
		size1 = A.N;
		WTimeStart = MPI_Wtime();
		Transposition(BT, B);
		SerialMultiplication(A, BT, D);
		WTimeEnd = MPI_Wtime();
		serialTime = WTimeEnd - WTimeStart;
		WTimeStart = MPI_Wtime();
		/*PrintMatrix(SizeM, A);
		PrintMatrix(SizeM, BT);
		PrintMatrix(SizeM, B);*/

	}
	MPI_Bcast(&nzA, 1, MPI_UNSIGNED_LONG, ROOT, MPI_COMM_WORLD);
	MPI_Bcast(&nzB, 1, MPI_UNSIGNED_LONG, ROOT, MPI_COMM_WORLD);
	MPI_Bcast(&size1, 1, MPI_UNSIGNED_LONG, ROOT, MPI_COMM_WORLD);


	if (rankP != ROOT) {

		InitializeMatrix(SizeM, SizeM * NNZRow, A);
		InitializeMatrix(SizeM, SizeM * NNZRow, B);
		InitializeMatrix(SizeM, SizeM * NNZRow, BT);
	}


	MPI_Bcast(A.Value, nzA, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
	MPI_Bcast(A.Col, nzA, MPI_INT, ROOT, MPI_COMM_WORLD);
	MPI_Bcast(A.RowIndex, size1 + 1, MPI_INT, ROOT, MPI_COMM_WORLD);

	MPI_Bcast(BT.Value, nzB, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
	MPI_Bcast(BT.Col, nzB, MPI_INT, ROOT, MPI_COMM_WORLD);
	MPI_Bcast(BT.RowIndex, size1 + 1, MPI_INT, ROOT, MPI_COMM_WORLD);


	parallelMultiplicate(C, A, BT);



	if (rankP == ROOT) {
		WTimeEnd = MPI_Wtime();
		parallelTime = WTimeEnd - WTimeStart;
		printf("\n\nTIME SERIAL: %lf\n", serialTime);

		printf("TIME IN PARALLEL:  %lf\n", parallelTime);

		printf("\n\nA \n");
		PrintMatrix(SizeM, A);
		printf("\n\nB \n");
		PrintMatrix(SizeM, B);
		printf("\n\nB_t \n");
		PrintMatrix(SizeM, BT);
		printf("\n\nC \n");
		PrintMatrix(SizeM, D);
		printf("\n");
	}
	MPI_Finalize();
	return 0;

}


void parallelMultiplicate(crsMatrix& result, crsMatrix A, crsMatrix B) {

	
	int size = A.N;
	size_t dropNZ = 0;
	int procPartSize = (size / sizeP);

	int* dropBuffCols = (int*)malloc(sizeof(int) * size);
	double* dropBuffValue = (double*)malloc(sizeof(double) * size);

	
	int* row_index = (int*)malloc(sizeof(int) * (size + 1));
	int iBuff = 0;

	int col, index, bcol;
	double sum;
	int* tmp = (int*)malloc(sizeof(int) * size);

	row_index[0] = 0;
	///
	for (int i = rankP * procPartSize; i < (rankP + 1) * procPartSize; i++) {
		memset(tmp, -1, size * sizeof(int));

		\
			for (int j = A.RowIndex[i]; j < A.RowIndex[i + 1]; j++) {
				col = A.Col[j];
				tmp[col] = j;
			}


		for (int j = 0; j < size; j++) {
			sum = 0.0;
			for (int k = B.RowIndex[j]; k < B.RowIndex[j + 1]; k++) {


				bcol = B.Col[k];
				index = tmp[bcol];

				if (index != -1) {
					sum += A.Value[index] * B.Value[k];
				}
			}


			if (fabs(sum) > EPS) {

				dropBuffCols[iBuff] = j;
				dropBuffValue[iBuff] = sum;
				row_index[iBuff]++;
				
				iBuff++;
				
				dropNZ++;

			
			}


		}
		
		row_index[i + 1 - rankP * procPartSize] = dropNZ;
	}


	int* displs = NULL;
	int* rcounts = NULL;

	if (rankP == ROOT) {
		displs = (int*)malloc(sizeP * sizeof(int));
		rcounts = (int*)malloc(sizeP * sizeof(int));
	}

	MPI_Gather((int*)&dropNZ, 1, MPI_INT, rcounts, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
	size_t nZ;

	if (rankP == ROOT) {


		nZ = rcounts[0];
		displs[0] = 0;
		for (int i = 1; i < sizeP; i++) {
			nZ += rcounts[i];
			displs[i] = displs[i - 1] + rcounts[i - 1];
		}
		InitializeMatrix(size, nZ, result);
	}


	MPI_Gatherv(dropBuffValue, dropNZ, MPI_DOUBLE, result.Value, rcounts, displs, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

	MPI_Gatherv(dropBuffCols, dropNZ, MPI_INT, result.Col, rcounts, displs, MPI_INT, ROOT, MPI_COMM_WORLD);


	MPI_Gather((row_index + 1),
		procPartSize,
		MPI_INT,
		(result.RowIndex + 1),
		procPartSize,
		MPI_INT,
		ROOT,
		MPI_COMM_WORLD);


	if (rankP == ROOT) {
		int endIndex = result.RowIndex[procPartSize];
		for (int i = 1; i < sizeP; i++) {
			for (int j = 1 + i * procPartSize; j <= procPartSize * (i + 1); j++) {
				result.RowIndex[j] += endIndex;
			}
			endIndex = result.RowIndex[procPartSize * (i + 1)];
		}

		result.RowIndex[0] = 0;
		free(tmp);
		
	}
	return;
}