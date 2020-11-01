#include "mpi.h"
#include <stdio.h>

#define totalsize 8
#define mysize totalsize/4
#define steps 10

int main(int argc, char* argv[])
{
int myid, numprocs, n, i, j;
float a[totalsize][mysize + 2], b[totalsize][mysize + 2], tmp[mysize][totalsize], c[totalsize][totalsize];
float temp[totalsize];
int begin_col, end_col;
MPI_Status status;


for (i = 0; i < totalsize; i++)
{
for (j = 0; j <= totalsize; j++)
c[i][j] = 0;
}

for (int j = 0; j < mysize; j++)
for (int i = 0; i < totalsize; i++)
tmp[j][i] = 8;

MPI_Init(&argc, &argv);


MPI_Comm_rank(MPI_COMM_WORLD, &myid);
MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
fprintf(stderr, "Process %d of %d is alive.\n", myid, numprocs);


//NOTE: totalsize, mysize+2
for (j = 0; j < mysize + 2; j++)
for (i = 0; i < totalsize; i++)
a[i][j] = 0.0;

if (myid == 0)
for (i = 0; i < totalsize; i++)
a[i][1] = 8.0;

if (myid == 3)
for (i = 0; i < totalsize; i++)
a[i][mysize] = 8.0;

for (i = 1; i < mysize + 1; i++) {
a[0][i] = 8.0;
a[totalsize - 1][i] = 8.0;
}
/* Jacobi */
for (n = 1; n <= steps; n++)
{

if (myid < 3)
{
MPI_Recv(&temp[0], totalsize, MPI_FLOAT, myid + 1, 10, MPI_COMM_WORLD, &status);
for (i = 0; i < totalsize; i++)
a[i][mysize + 1] = temp[i];

}

if (myid > 0)
{
for (i = 0; i < totalsize; i++)
temp[i] = a[i][1];
MPI_Send(&temp[0], totalsize, MPI_FLOAT, myid - 1, 10, MPI_COMM_WORLD);
}

if (myid < 3)
{
for (i = 0; i < totalsize; i++)
temp[i] = a[i][mysize];
MPI_Send(&temp[0], totalsize, MPI_FLOAT, myid + 1, 10, MPI_COMM_WORLD);
}

if (myid > 3)
{
MPI_Recv(&temp[0], totalsize, MPI_FLOAT, myid - 1, 10, MPI_COMM_WORLD, &status);
for (i = 0; i < totalsize; i++)
a[i][0] = temp[i];

}

begin_col = 1;
end_col = mysize;

if (myid == 0)
begin_col = 2;
if (myid == 3)
end_col = mysize - 1;

for (j = begin_col; j <= end_col; j++)
for (i = 1; i < totalsize - 1; i++)
b[i][j] = 0.25*(a[i][j + 1] + a[i][j - 1] + a[i + 1][j] + a[i - 1][j]);

for (i = 1; i < totalsize - 1; i++)
for (j = begin_col; j <= end_col; j++)
{
a[i][j] = b[i][j];
}
}
int loc = begin_col;
if (0 == myid)
loc--;
for (i = 1; i < totalsize - 1; i++)
for (j = begin_col; j <= end_col; j++)
{
tmp[j - loc][i] = a[i][j];
}

MPI_Barrier(MPI_COMM_WORLD);

MPI_Gather(tmp, 16, MPI_FLOAT, c, 16, MPI_FLOAT, 0, MPI_COMM_WORLD);


if (0 == myid)
{
fprintf(stderr, "\nProcess %d :\n", myid);
for (int j = 0; j < mysize; j++)
{
for (int i = 0; i < totalsize; i++)
fprintf(stderr, "%.2f\t", tmp[j][i]);
fprintf(stderr, "\n");
}
fprintf(stderr, "\n");

fprintf(stderr, "\nResults after collection\n");
for (int i = 0; i < totalsize; i++)
{
for (int j = 0; j < totalsize; j++)
fprintf(stderr, "%.2f\t", c[i][j]);
fprintf(stderr, "\n");
}
fprintf(stderr, "\n");
}
MPI_Finalize();
}
