#include "mpi.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

void allreduce(void* sbuf, void* rbuf, int count, MPI_Datatype datatype, MPI_Op operation, MPI_Comm comm)
{
	int ProcRank, ProcNum;
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	MPI_Status status;

	if (ProcRank == 0)
	{
		for (int j = 1; j < ProcNum; j++)
		{
			MPI_Status status;
			MPI_Recv(rbuf, count, datatype, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			if (operation == MPI_SUM)
			{
				for (int i = 0; i < count; i++)
				{
					if (datatype == MPI_INT)
						((int*)sbuf)[i] = ((int*)sbuf)[i] + ((int*)rbuf)[i];
					if (datatype == MPI_DOUBLE)
						((double*)sbuf)[i] += ((double*)rbuf)[i];
				}
			}

			if (operation == MPI_MIN)
			{
				for (int i = 0; i < count; i++)
				{
					if (datatype == MPI_INT)
						if (((int*)sbuf)[i] > ((int*)rbuf)[i])
							((int*)sbuf)[i] = ((int*)rbuf)[i];
					if (datatype == MPI_DOUBLE)
						if (((double*)sbuf)[i] > ((double*)rbuf)[i])
							((double*)sbuf)[i] = ((double*)rbuf)[i];
				}
			}
		}
		for (int i = 1; i < ProcNum; i++)
		{
			MPI_Send(sbuf, count, datatype, i, 1, MPI_COMM_WORLD);
		}
		for (int i = 0; i < count; i++)
		{
			if (datatype == MPI_INT)
				fprintf(stdout, "%d ", ((int*)sbuf)[i]);

			if (datatype == MPI_DOUBLE)
				fprintf(stdout, "%lf ", ((double*)sbuf)[i]);
			fflush(stdout);
		}
	}
	else
	{
		MPI_Status status;
		MPI_Send(sbuf, count, datatype, 0, 0, MPI_COMM_WORLD);
		MPI_Recv(rbuf, count, datatype, 0, 1, MPI_COMM_WORLD, &status);
	}
}

int main(int argc, char* argv[])
{
	int ProcRank, ProcNum;
	int n = atoi(argv[1]);
	double starttime, endtime;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	starttime = MPI_Wtime();

	int* a = new int[n];
	int* b = new int[n];
	for (int i = 0; i < n; i++)
		b[i] = 0;

	for (int i = 0; i < n; i++)
		a[i] = 3;
	//a[i] = rand()%10;

	allreduce(a, b, n, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	//allreduce(a, b, n, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
	if (ProcRank == 0)
	{
		endtime = MPI_Wtime();
		fprintf(stdout, "\n");
		fprintf(stdout, "%lf", endtime - starttime);
		fflush(stdout);
	}
	MPI_Finalize();
}

