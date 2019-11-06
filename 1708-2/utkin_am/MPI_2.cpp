#include "mpi.h"
#include <iostream> 
#include "stdlib.h"
#include "windows.h"


struct MyString {
	char* s;
	int l;
};
struct ReturnedVal {
	int numOfWords;
	int startSimbol;
	int endSimbol;
};

//void createString(struct MyString ms, char* s, int l);
void createString(struct MyString* ms, char* s, int l) {
	ms->s = s;//(char*)malloc(sizeof(char)*l);
	ms->l = l;
}
void splitStrings(char* s, int lenOfStr, struct MyString* pStr, int numOfProc) {
	int i = 0;
	for (i; i < numOfProc - 1; i++)
		createString(&pStr[i], s + i * (lenOfStr / numOfProc), (lenOfStr / numOfProc));
	if ((lenOfStr % (lenOfStr / numOfProc)) != 0)
		createString(&pStr[i], s + i * (lenOfStr / numOfProc), lenOfStr / numOfProc + (lenOfStr % (lenOfStr / numOfProc)));
	else
		createString(&pStr[i], s + i * (lenOfStr / numOfProc), (lenOfStr / numOfProc));


}

int main(int argc, char* argv[])
{

	MPI_Status status;
	int k; // for counting
	struct MyString* pStr=0;
	struct MyString l_pStr;
	struct ReturnedVal rv; // for return
	struct ReturnedVal* sv; //for saving in 0 proc
	//int lenOfStr;
	int i = 0, n;
	int numprocs, myid;
	char* curStr=0;
	double startwtime = 0.0, endwtime;
	int  namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Get_processor_name(processor_name, &namelen);


	fprintf(stdout, "Process %d of %d is on %s\n",
		myid, numprocs, processor_name);
	fflush(stdout);


	if (myid == 0) {
		n = 1000000000;
		curStr = (char*)malloc(sizeof(char) * n);
		for (i = 0; i < n; i++) {
			curStr[i] = 'a';
			if (0 == i % 45)
				curStr[i] = ' ';
		}
		pStr = (struct MyString*)malloc(sizeof(struct MyString) * numprocs);
		fprintf(stdout, "length of curString: %d \n", n);
		fflush(stdout);
		splitStrings(curStr, n, pStr, numprocs);
		startwtime = MPI_Wtime();
		l_pStr = pStr[0];
		for (i = 1; i < numprocs; i++) {
			MPI_Send(&pStr[i].l, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(pStr[i].s, pStr[i].l, MPI_CHAR, i, 2, MPI_COMM_WORLD);
		}
	}
	else {
		MPI_Recv(&l_pStr.l, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		/*fprintf(stdout, "length of recvString(%d): %d \n",myid, l_pStr.l);
		fflush(stdout);*/
		l_pStr.s = (char*)malloc(sizeof(char) * l_pStr.l);
		MPI_Recv(l_pStr.s, l_pStr.l, MPI_CHAR, 0, 2, MPI_COMM_WORLD, &status);
		/*fprintf(stdout, "%c \n", l_pStr.s[l_pStr.l - 1]);
		fflush(stdout);*/
	}
	i = 0;
	k = 0;
	rv.numOfWords = 0;
	rv.endSimbol = 0;
	rv.startSimbol = 0;
	while (i < l_pStr.l && l_pStr.s[i] != '\0') {
		/*printf("%c", l_pStr.s[i]);
		fflush(stdout);*/
		if (((l_pStr.s[i] >= 'a') && (l_pStr.s[i] <= 'z'))
			|| ((l_pStr.s[i] >= 'A') && (l_pStr.s[i] <= 'Z'))) {
			k++;
			if (i == 0)
				rv.startSimbol = 1;
		}
		else {
			if (k != 0)
				rv.numOfWords++;
			k = 0;
		}
		i++;
	}
	if (k != 0) {
		rv.endSimbol = 1;
		rv.numOfWords++;
	}
	if (myid != 0) {
		MPI_Send(&rv, sizeof(struct ReturnedVal), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	}
	else {
		sv = (struct ReturnedVal*)malloc(sizeof(struct ReturnedVal) * numprocs);
		sv[0] = rv;
		for (i = 1; i < numprocs; i++) {
			MPI_Recv(&rv, sizeof(struct ReturnedVal), MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
			sv[i] = rv;
		}
		rv.numOfWords = sv[0].numOfWords;
		for (i = 1; i < numprocs; i++) {
			rv.numOfWords += sv[i].numOfWords;
			if (sv[i - 1].endSimbol && sv[i].startSimbol)
				rv.numOfWords--;
		}
		endwtime = MPI_Wtime();
		printf("\nnumber of words: %d \n", rv.numOfWords);
		printf("time = %f\n", endwtime - startwtime);
		fflush(stdout);
		free(curStr);
		free(pStr);
		free(sv);
	}

	MPI_Finalize();
	return 0;
}



