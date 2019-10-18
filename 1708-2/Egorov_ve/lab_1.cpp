#include "pch.h"
#include "mpi.h"
#include <iostream>
#include <random>
#include <ctime>
#include <numeric>
#include <vector>
#include <algorithm>
#include <functional>
 
using namespace std;
 
int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int mpi_rank, mpi_size; 
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size); 
    int width, height;
    int **matrix = NULL;
    int *tmp = NULL;
    long *result = NULL;
    double times;
    MPI_Status Status;
    cout << "mpi_size=" << mpi_size << endl;
    if (mpi_rank == 0)
    {
 
        cin >> width;
        cin >> height;
        srand(time(0));
 
        cout << endl;
        matrix = new int*[height*width];
        result = new long[height*width];
        for (int i = 0; i < width; i++) {
            matrix[i] = new int[width];
            result[i] = 0;                 
        }
 
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                matrix[i][j] = rand() % 10; 
                cout << matrix[i][j] << " "; 
            }
            cout << endl;
        }
        cout << endl;
    }
 
    MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
 
    if (mpi_rank == 0)
    {
        for (int i = 0; i < width; i++)
        {
            if (i%mpi_size != 0)
                MPI_Send(matrix[i], height, MPI_INT, i%mpi_size, i, MPI_COMM_WORLD);
        }
    }
 
    times = MPI_Wtime();
 
    if (mpi_rank != 0)
    {
        tmp = new int[width];
        for (int i = mpi_rank; i < width; i += mpi_size)
        {
            MPI_Recv(tmp, height, MPI_INT, 0, i, MPI_COMM_WORLD, &Status);
            int sum = 0;
            for (int j = 0; j < height; j++)
                sum += tmp[j];
            MPI_Send(&sum, 1, MPI_INT, 0, i, MPI_COMM_WORLD); 
        }
 
    }
    else
    {
        for (int i = mpi_rank; i < width; i += mpi_size)
        {
            long sum = 0;
            for (int j = 0; j < height; j++)
            {
                sum += matrix[i][j];
            }
            result[i] = sum;
        }
    }
 
    fprintf(stdout, "End(%d): %2.10f\n", mpi_rank, MPI_Wtime() - times);
    fflush(stdout);
 
    if (mpi_rank == 0)
    {
        for (int i = 0; i < width; i++)
        {
            if (i%mpi_size)
            {
                MPI_Recv(result + i, 1, MPI_LONG, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &Status);
            }
        }
 
        int s = 1;
        for (int i = 0; i < width; i++)
        {
            cout << s << "str: " << result[i] << endl;
            s++;
        }
    }
    getchar();
    delete matrix, result, tmp;
    MPI_Finalize();
    return 0;
}