#include "pch.h"
#include "mpi.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <ctime>
#include <cmath>
#include <vector>
#include <string>
using namespace std;
 
void sendr(int ProcSize, int neproc, int result, int Side) {
    int buffer[10]{ 1 }, buffer2[10]{ 1 }, rank,size,f;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status status;
    int i = 0;
 
    if (Side == 0) {
 
        for (int i = 0; i < 1; i++) {
            if (rank == 4)
                MPI_Sendrecv(&buffer[0], 2, MPI_INT, size, i, &buffer2[0], 2, MPI_INT, 0, i, MPI_COMM_WORLD, &status);
            cout << 0 << "at"; fflush(stdout);;
            cout << size << "to"; fflush(stdout);;
        }
 
        for (int j = size; j > neproc; j--) {
            i = j;
            if ((rank == j) || (rank == i))
                MPI_Sendrecv(&buffer[0], 2, MPI_INT, j, j, &buffer2[0], 2, MPI_INT, i, j, MPI_COMM_WORLD, &status);
                cout << i << "at"; fflush(stdout);;
                cout << j << "to"; fflush(stdout);;
        }
    }
 
 
    if (Side == 1) {
        for (int j = 0; j < neproc; j++) {
            i = j;
            if ((rank == j) || (rank == i))
                MPI_Sendrecv(&buffer[0], 2, MPI_INT, j, j, &buffer2[0], 2, MPI_INT, i, j, MPI_COMM_WORLD, &status);
                cout << i << "at"; fflush(stdout);;
                cout << j << "to"; fflush(stdout);;
        }
    }
}
 
 
bool IsRingTopology(MPI_Comm comm) {
    int status;
    MPI_Topo_test(comm, &status);
    if (status != MPI_CART)
        return false;
 
    int ndims;
    MPI_Cartdim_get(comm, &ndims);
    if (ndims != 1)
        return false;
 
    std::vector<int> dims(ndims), periods(ndims), coords(ndims);
    MPI_Cart_get(comm, ndims, dims.data(), periods.data(), coords.data());
    if (periods[0] != 1)
        return false;
 
    return true;
}
 
int main(int argc, char* argv[]) {
    double TimeStartWork, TimeEndWork, TimeStartWork2, TimeEndWork2;
    int neproc = 0, rigth = 0, left = 0, result = 0;
    int Side;
    MPI_Comm oldcomm = MPI_COMM_WORLD;
    MPI_Comm ringcomm;
    int oldSize, rankProc;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rankProc);
    MPI_Comm_size(MPI_COMM_WORLD, &oldSize);
    std::vector<int> dims(1), periods(1);
    neproc = atoi(argv[1]);
    MPI_Comm_size(oldcomm, &dims[0]);
    periods[0] = 1;
    TimeStartWork = MPI_Wtime();
    MPI_Cart_create(oldcomm, 1, dims.data(), periods.data(), 0, &ringcomm);
 
    bool IsW = IsRingTopology(ringcomm);
    TimeEndWork = MPI_Wtime();
    TimeStartWork2 = MPI_Wtime();
    if (rankProc == 0) {
        for (int j = 0; j < neproc; j++)
            rigth++;
        for (int i = oldSize; i > neproc; i--)
            left++;
        if (rigth > left) {
            Side = 0;
            result = left;
        }
        else if (rigth < left) {
            Side = 1;
            result = rigth;
        }
        else {
            Side = 1;
            result = left;
        }
        cout << "\n" << left << ":left \n"; fflush(stdout);;
        cout << "\n" << rigth << ":rigth \n"; fflush(stdout);;
    }
    sendr(oldSize, neproc, result, Side);
    if (rankProc == 0) {
        TimeEndWork2 = MPI_Wtime();
        cout << "\nTime In Main = " << TimeEndWork2 - TimeStartWork2 << " sec" << endl;
        cout << "\nTime In Standart With Test = " << TimeEndWork - TimeStartWork << " sec" << endl;
    }
    MPI_Finalize();
    return 0;
}