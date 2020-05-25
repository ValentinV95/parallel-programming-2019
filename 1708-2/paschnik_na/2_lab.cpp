#include <mpi.h>
#include <vector>
#include <random>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

int procRank, procSize;

void broadcast(void* buf, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {

    if (procRank == root) {
        for (int i = 0; i < procSize; i++) {
            if (i != procRank) {
                MPI_Send(buf, count, datatype, i, 0, comm);
            }
        }
    }
    else {
        MPI_Recv(buf, count, datatype, root, 0, comm, MPI_STATUS_IGNORE);
    }
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Status status;

    double begin = .0;
    double end = .0;
    begin = MPI_Wtime();   
    MPI_Comm_size(MPI_COMM_WORLD, &procSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
    
    int data;
    int sum = 0;

    if (procRank == 0) {
        data = 7;
    }
    broadcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD);
    data++;

    MPI_Reduce(&data, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    end = MPI_Wtime();
    if (procRank == 0) {
        cout << sum << " = " << data * procSize << ". Time spent = " << end - begin << endl;
        cout << "Witn " << procSize << " processes" << endl;
    }
    MPI_Finalize();
}