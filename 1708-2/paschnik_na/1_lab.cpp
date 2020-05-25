#include "stdio.h"
#include "mpi.h"
#include "stdlib.h"
#include "math.h"
#include <string>
#include <iostream>
#include <time.h>
using namespace std;

int ProcNum, ProcRank;
int globalCounter;
string str;
char c;

int getCountInStr(std::string str, char c) {
    int count = 0;
    size_t len = str.length();
    for (size_t i = 0; i < len; i++) {
        if (str[i] == c)
            count++;
    }
    return count;
}

void getCountForMainProc(int countProc, string localStr, int step) {
    int localCounter = 0;
    MPI_Status status;
    int counter;
    int len = static_cast<int>(str.length());
    for (int proc = 1; proc < ProcNum - countProc; proc++) {
        counter = 0;
        localStr = "";
        counter = step * (proc - 1);
        for (int i = 0; i < step; i++)
            localStr += str[i + counter];
        MPI_Send(&localStr[0], step, MPI_CHAR, proc, 0, MPI_COMM_WORLD);
    }
    for (int proc = ProcNum - countProc; proc < ProcNum; proc++) {
        localStr = "";
        int counter = len + (step + 1) * (proc - ProcNum);
        for (int i = 0; i < step + 1; i++)
            localStr += str[i + counter];
        MPI_Send(&localStr[0], (step + 1), MPI_CHAR, proc, 2, MPI_COMM_WORLD);
    }
    for (int proc = 1; proc < ProcNum; proc++) {
        MPI_Recv(&localCounter, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
        globalCounter += localCounter;
    }
}

void getCountForOtherProc(int countProc, string localStr, int step) {
    int localCounter = 0;
    MPI_Status status;
    if (ProcRank > 0 && ProcRank < ProcNum - countProc)
        MPI_Recv(&localStr[0], step, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
    if (ProcRank >= ProcNum - countProc && ProcRank < ProcNum) {
        localStr.resize(step + 1);
        MPI_Recv(&localStr[0], step + 1, MPI_CHAR, 0, 2, MPI_COMM_WORLD, &status);
    }
    localCounter = getCountInStr(localStr, c);
    MPI_Send(&localCounter, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
}

int getCountCharInStrWithMpi() {
    int localCounter = 0;
    int len = static_cast<int>(str.length());
    if (ProcNum < 2) {
        int count = getCountInStr(str, c);
        return count;
    }
    int step = len / (ProcNum - 1);
    int countProc = len % (ProcNum - 1);

    MPI_Status status;
    string localStr("", step);

    MPI_Barrier(MPI_COMM_WORLD);

    if (ProcRank == 0) {
        getCountForMainProc(countProc, localStr, step);
    }
    else {
        getCountForOtherProc(countProc, localStr, step);
    }
    return globalCounter;
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    MPI_Status status;

    globalCounter = 0;
    double begin = .0;
    double end = .0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    str = "asssaaaddaaa";
    c = 'a';

    begin = MPI_Wtime();
    int count = getCountCharInStrWithMpi();
    end = MPI_Wtime();
    if (ProcRank == 0) {
        cout << "In the "<< str << " line, there are " << count << " repeated characters " << c << ". Time spent = " << end - begin << endl;
        cout << "Witn " << ProcNum << " processes" << endl;
    }
    MPI_Finalize();
} 