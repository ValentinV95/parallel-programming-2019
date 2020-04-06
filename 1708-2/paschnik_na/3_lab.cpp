#include <mpi.h>
#include <vector>
#include <random>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

int procRank, procSize;

std::vector<int> randomGenerateVector(int sizeVector) {
    if (sizeVector < 1) {
        throw "ErrorLength";
    }
    std::mt19937 generation;
    generation.seed(static_cast<unsigned int>(time(0)));
    std::vector<int> vec(sizeVector);
    for (int i = 0; i < sizeVector; i++) {
        vec[i] = generation() % 1000;
    }
    return vec;
}

void quickSortWithoutMPI(std::vector<int>* vec, int left, int right) {
    if (vec->size() < 1) {
        throw "ErrorLentgh";
    }
    int l = left, r = right;
    int pivot = (*vec)[(left + right) / 2];
    int temp;
    do {
        while ((*vec)[l] < pivot)
            l++;
        while ((*vec)[r] > pivot)
            r--;
        if (l <= r) {
            temp = (*vec)[l];
            (*vec)[l] = (*vec)[r];
            (*vec)[r] = temp;
            l++;
            r--;
        }
    } while (l <= r);

    if (l < right) {
        quickSortWithoutMPI(&(*vec), l, right);
    }
    if (left < r) {
        quickSortWithoutMPI(&(*vec), left, r);
    }
}

void quickSortWithMPI(std::vector<int>* vec) {
    int sizeVector = vec->size();

    MPI_Comm_size(MPI_COMM_WORLD, &procSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

    int blockData = sizeVector / procSize;
    std::vector<int> localVec(blockData);

    MPI_Scatter(&(*vec)[0], blockData, MPI_INT, &localVec[0],
        blockData, MPI_INT, 0, MPI_COMM_WORLD);

    quickSortWithoutMPI(&localVec, 0, blockData - 1);

    MPI_Gather(&localVec[0], blockData, MPI_INT, &(*vec)[0], blockData, MPI_INT, 0, MPI_COMM_WORLD);

    if (procRank == 0) {
        quickSortWithoutMPI(&(*vec), 0, sizeVector - 1);
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
    
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> array(10);
    if (rank == 0) {
        array = randomGenerateVector(10);
    }

    if (procRank == 0) {
        cout << "Your vector:" << endl;

        for (int i = 0; i < 10; i++) {
            cout << array[i] << " ";
        }
        cout << endl;
        cout << endl;
    }



    quickSortWithMPI(&array);
    
    end = MPI_Wtime();
    if (procRank == 0) {
        cout << "Sorted vector:" << endl;
        for (int i = 0; i < 10; i++) {
            cout << array[i] << " ";
        }
        cout << endl;
        cout << endl;
        cout << "Time spent = " << end - begin << endl;
        cout << "Witn " << procSize << " processes" << endl;
    }
    MPI_Finalize();
}