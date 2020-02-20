#include "mpi.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <ctime>
#include <cmath>
#include <string>
#include <bitset>
using namespace std;
 
void sendr(int ProcSize, int neproc, MPI_Comm New_Comm) {
    int buffer[10]{ 1 }, buffer2[10]{ 1 }, rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;
    int i = 0;
    for (int j = 0; j <= neproc; j++) {
        i = j;
        if ((rank == j)||(rank==i))
            MPI_Sendrecv(&buffer[0], 2, MPI_INT, j, j, &buffer2[0], 2, MPI_INT, i, j, MPI_COMM_WORLD, &status);
                cout << i << "at"; fflush(stdout);;
                cout << j << "to"; fflush(stdout);;
    }
}
 
bool testLinearTopology(MPI_Comm commLinear, int new_coords[]) {
    int rank, size;
    int sourceLess, sourceBig, destLess, destBig;
    int val, valFromLess, valFromBig;
    MPI_Status status;
    MPI_Comm_rank(commLinear, &rank);
    MPI_Comm_size(commLinear, &size);
    MPI_Cart_coords(commLinear, rank, 1, new_coords);
    val = new_coords[0];
    valFromLess = -1;
    valFromBig = -1;
    MPI_Cart_shift(commLinear, 0, 1, &sourceLess, &destBig);
    MPI_Cart_shift(commLinear, 0, -1, &sourceBig, &destLess);
    MPI_Sendrecv(&val, 1, MPI_INT, destBig, 4, &valFromLess, 1, MPI_INT,
        sourceLess, 4, commLinear, &status);
    MPI_Sendrecv(&val, 1, MPI_INT, destLess, 4, &valFromBig, 1, MPI_INT,
        sourceBig, 4, commLinear, &status);
    MPI_Comm_free(&commLinear);
 
    if ((rank + 1) != valFromBig) {
        if (new_coords[0] != size - 1) return false;
    }
    if ((rank - 1) != valFromLess) {
        if (new_coords[0] != 0) return false;
    }
    return true;
}
 
int main(int argc, char** argv) {
    double TimeStartWork, TimeEndWork, TimeStartWork2, TimeEndWork2;
    int size = 5;
    int neproc = 0;
    int new_coords[1];
    int reorder = 0;
    int dims[1]{ 0 }, periods[1]{ 0 };
    int oldSize, rankProc;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rankProc);
    MPI_Comm_size(MPI_COMM_WORLD, &oldSize);
    size = atoi(argv[1]);
    neproc = atoi(argv[2]);
    new_coords[atoi(argv[2])];
    if (size <= 0 || size > oldSize)
        MPI_Dims_create(oldSize, 1, dims);
    else
        MPI_Dims_create(size, 1, dims);
    MPI_Comm commLinear;
    MPI_Cart_create(MPI_COMM_WORLD, 1, dims, periods, reorder, &commLinear);
    bool test = testLinearTopology(commLinear, new_coords);
    if (rankProc == 0) {
        TimeStartWork = MPI_Wtime();
        cout << test; fflush(stdout);;
        TimeEndWork = MPI_Wtime();
        cout << "\nTime In Standart = " << TimeEndWork - TimeStartWork << " sec" << endl;
    }
    TimeStartWork2 = MPI_Wtime();
    sendr (oldSize, neproc, commLinear);
    if (rankProc == 0) {
        TimeEndWork2 = MPI_Wtime();
        cout << "\nTime In Main = " << TimeEndWork2 - TimeStartWork2 << " sec" << endl;
    }
    MPI_Finalize();
    return 0;
}
