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
    MPI_Init(&argc, &argv); //Ð¸Ð½Ð¸Ñ†Ð¸Ð°Ð»Ð¸Ð·Ð°Ñ†Ð¸Ñ
    int mpi_rank, mpi_size; //Ð¿ÐµÑ€ÐµÐ¼ÐµÐ½Ð½Ñ‹Ðµ ÐºÐ¾Ð»-Ð²Ð° Ð¿Ñ€Ð¾Ñ†ÐµÑÑÐ¾Ð² Ð¸ Ñ€Ð°Ð½Ð³Ð° Ð¿Ñ€Ð¾Ñ†ÐµÑÑÐ°
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank); //Ð·Ð°Ð¿Ð¸ÑÑ‹Ð²Ð°ÐµÐ¼ Ñ€Ð°Ð½Ð³ Ð¿Ñ€Ð¾Ñ†ÐµÑÑÐ° Ð² mpi_rank
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size); 
    int width, height;
    int **matrix = NULL;
    int *tmp = NULL; //Ð´Ð¾Ð¿ Ð¼Ð°Ñ‚Ñ€Ð¸Ñ†Ð° Ð´Ð»Ñ ÑÑƒÐ¼Ð¼Ð¸Ñ€Ð¾Ð²Ð°Ð½Ð¸Ñ
    long *result = NULL; //Ð¼Ð°Ñ‚Ñ€Ð¸Ñ†Ð° ÑÑƒÐ¼Ð¼
    double times;
    MPI_Status Status;
    cout << "mpi_size=" << mpi_size << endl; //Ð²Ñ‹Ð²Ð¾Ð´ ÐºÐ¾Ð»-Ð²Ð° Ð´ÐµÐ¹ÑÑ‚Ð²ÑƒÑŽÑ‰Ð¸Ð¹ Ð¿Ñ€Ð¾Ñ†ÐµÑÑÐ²Ð¾ Ð² Ð¿Ñ€Ð¾Ð³Ñ€Ð°Ð¼Ð¼Ðµ
    if (mpi_rank == 0)
    {
 
        cin >> width;
        cin >> height; //Ð²Ð²Ð¾Ð´Ð¸Ð¼ ÐºÐ¾Ð»-Ð²Ð¾ ÑÑ‚Ñ€Ð¾Ðº
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