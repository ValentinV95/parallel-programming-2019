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
#include <string.h>
#include <stdlib.h>

using namespace std;

int countOfPrintSymbol(const char *str, int size)
{
	int count = 0;

	for (int i = 0; i <= size; ++i) 
	{
		if ((' ' != str[i]) && ('\0' != str[i]) && ('\n' != str[i]) && ('\t' != str[i]) && ((int)str[i] >= 33) && ((int)str[i] <= 255))
			count++;
	}
	return count;
}

int main(int argc, char *argv[]) 
{
	clock_t timeSec = 0;
	srand(time(NULL));
	int sizeStr = rand();
	char *str = (char *)malloc(sizeStr * sizeof(char));

	for (int i = 0; i < sizeStr - 1; ++i)
	{
		str[i] = (char)(rand() % 255 + 1);
	}

	str[sizeStr - 1] = '\0';
	int procNum, procRank;							// procNum - number of processes; procRank - rank of current process 
	int lenStr = 0;
	int beginStr = 0, endStr = 0;
	int lenMid = 0, lenEnd = 0;
	int count = 0;
	int numRecvStr = 0;
	int *recvCount = (int *)malloc(sizeof(int*));

	MPI_Status status;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &procNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

	lenStr = strlen(str);

	if (0 == procRank)
		cout << "Length of string = " << lenStr << endl;

	if (0 == procRank)
	{
		if (0 != lenStr % procNum)
		{
			lenMid = lenStr / procNum;
			lenEnd = lenStr - lenMid * (procNum - 1);

			for (int i = 1; i < procNum; i++)
			{
				if (procNum - 1 != i)
				{
					beginStr = lenMid * i;
					endStr = beginStr + lenMid - 1;
					MPI_Send(str + beginStr, endStr - beginStr + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
				}
				else
				{
					MPI_Send(str + lenMid * (procNum - 1), lenEnd, MPI_CHAR, i, 0, MPI_COMM_WORLD);

				}
			}
		}
		else
		{
			lenMid = 1 < procNum ? lenStr / (procNum - 1) : lenMid = lenStr;

			for (int i = 1; i < procNum; i++)
			{
				beginStr = lenMid * i;
				endStr = beginStr + lenMid - 1;
				MPI_Send(str + beginStr, endStr - beginStr + 1, MPI_CHAR, i, procRank, MPI_COMM_WORLD);
			}
		}
		beginStr = lenMid * 0;
		endStr = beginStr + lenMid;

		count = countOfPrintSymbol(str, endStr - beginStr);

		while (procNum - 1 != numRecvStr) {
			MPI_Recv(recvCount, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			count += *recvCount;
			numRecvStr++;
		}

		free(recvCount);

		timeSec = clock();

		cout << "The process " << procRank << " did his work in " << ((float)timeSec / CLOCKS_PER_SEC) << " sec";
	}
	else 
	{
		if (0 != lenStr % procNum) 
		{
			lenMid = lenStr / procNum;
			lenEnd = lenStr - lenMid * (procNum - 1);
		}
		else 
		{
			lenMid = lenStr / (procNum - 1);
			lenEnd = lenMid;
		}
		int size = procRank != procNum - 1 ? lenMid : lenEnd;
		char *recvStr = (char *)malloc(size);
		MPI_Recv(recvStr, size, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		count = countOfPrintSymbol(recvStr, size);
		MPI_Send(&count, 1, MPI_INT, 0, procRank, MPI_COMM_WORLD);
		timeSec = clock();

		cout << "The process " << procRank << " did his work in " << ((float)timeSec / CLOCKS_PER_SEC) << " sec";
	}

	MPI_Finalize();

	cout << endl;
	return 0;
}