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
    MPI_Init(&argc, &argv); //инициализация
    int mpi_rank, mpi_size; //переменные кол-ва процессов и ранга процесса
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank); //записываем ранг процесса в mpi_rank
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size); 
    int width, height;
    int **matrix = NULL;
    int *tmp = NULL; //доп матрица для суммирования
    long *result = NULL; //матрица сумм
    double times;
    MPI_Status Status;
    cout << "mpi_size=" << mpi_size << endl; //вывод кол-ва действующий процессво в программе
    if (mpi_rank == 0)
    {
 
        cin >> width;
        cin >> height; //вводим кол-во строк
        srand(time(0));
 
        cout << endl;
        matrix = new int*[height*width];
        result = new long[height*width];
        for (int i = 0; i < height; i++) {
            matrix[i] = new int[height];
            result[i] = 0;                 
        }
 
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
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
        for (int i = 0; i < height; i++)
        {
            if (i%mpi_size != 0)
                MPI_Send(matrix[i], width, MPI_INT, i%mpi_size, i, MPI_COMM_WORLD);
        }
    }
 
    times = MPI_Wtime();
 
    if (mpi_rank != 0)
    {
        tmp = new int[height];
        for (int i = mpi_rank; i < height; i += mpi_size)
        {
            MPI_Recv(tmp, width, MPI_INT, 0, i, MPI_COMM_WORLD, &Status);
            int sum = 0;
            for (int j = 0; j < width; j++)
                sum += tmp[j];
            MPI_Send(&sum, 1, MPI_INT, 0, i, MPI_COMM_WORLD); 
        }
 
    }
    else
    {
        for (int i = mpi_rank; i < height; i += mpi_size)
        {
            long sum = 0;
            for (int j = 0; j < width; j++)
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
        for (int i = 0; i < height; i++)
        {
            if (i%mpi_size)
            {
                MPI_Recv(result + i, 1, MPI_LONG, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &Status);
            }
        }
 
        int s = 1;
        for (int i = 0; i < height; i++)
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