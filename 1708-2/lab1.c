///lab1 reactangle method integral of cosx in a ~ b

#include "mpi.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

#define PI 3.141592654

int main(int argc, char** argv)
{
int rank,size;
MPI_Status status;
double a, b;

a = 0;
b = 2 * PI;

MPI_Init(&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &size);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);

int p_num;
p_num = size - 1;
int n_num;
n_num = 10;

double h;
h = (b - a) / p_num / n_num;
if (rank == size - 1)
{
int j = 0;
double fin_num = 0;
double tem_n;
for (j = 0; j < size - 1; j++)
{
MPI_Recv(&tem_n, 1, MPI_DOUBLE, j, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
fin_num = fin_num + tem_n;
}
printf(" cos(%f,%f) : %f\n ", a, b, fin_num);
}
else {
int j = 0;
double part_num = 0;
for (j = 0; j < n_num; j++)
{
part_num = part_num + cos(a + (rank *n_num+j)*h + h / 2)*h;
}
MPI_Send(&part_num, 1, MPI_DOUBLE, size - 1, 99, MPI_COMM_WORLD);
}

MPI_Finalize();
} 
