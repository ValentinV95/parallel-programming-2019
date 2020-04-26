#include <stdio.h>
#include "mpi.h"
#include "malloc.h"

#define n 4
#define BLOCK_LOW(id, p, n) ((id) * (n) / (p))
#define BLOCK_HIGH(id, p, n) (BLOCK_LOW((id)+1, p, n) - 1)
#define BLOCK_SIZE(id, p, n) (BLOCK_LOW((id)+1, p, n) - BLOCK_LOW((id), p, n))
#define BLOCK_OWNER(index, p, n) (((p) * ((index)+1) - 1) / (n))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
int NotIn(int id, int *picked);
struct {
double value;
int index;
}local, global;

int main(int argc, char *argv[])
{
int id, p;
MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &id);
MPI_Comm_size(MPI_COMM_WORLD, &p);

//double a[n][n+1] = {{4,6,2,-2,8},{2,0,5,-2,4},{-4,-3,-5,4,1},{8,18,-2,3,40}};
double a[n][n + 1] = { {2,1,-5,1,8},{1,-3,0,-6,9},{0,2,-1,2,-5},{1,4,-7,6,0} };

int i, j;
int index;

int *picked;
picked = (int *)malloc(sizeof(int) * n); // Record the selected line
for (i = 0; i < n; i++)
picked[i] = -1;

for (i = 0; i < n; i++)
{

if (NotIn(id, picked)) // Determine whether the line has been selected, if there is no selection, proceed to the next step
{
local.value = a[id][i];
local.index = id;
}
else
{
local.value = -100;
local.index = id;
}

MPI_Allreduce(&local, &global, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD); //global
// printf(" i = %d,id =%d,value = %f,index = %d\n",i,id,global.value,global.index);
picked[i] = global.index;

if (id == global.index)
{
MPI_Bcast(&global.index, 1, MPI_INT, id, MPI_COMM_WORLD);
}

MPI_Allgather(&a[id][0], n + 1, MPI_DOUBLE, a, n + 1, MPI_DOUBLE, MPI_COMM_WORLD);


if (NotIn(id, picked))
{
double temp = 0 - a[id][i] / a[picked[i]][i];
for (j = i; j < n + 1; j++)
{
a[id][j] += a[picked[i]][j] * temp;
}
}

}

MPI_Gather(&a[id][0], n + 1, MPI_DOUBLE, a, n + 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
//// Each thread solves the solution of the corresponding line, for example: the thread with thread number 0 only gets the solution of line 0, but the change of line 1, there is no way to get 0 thread, only 1 thread I knew it myself, so I need to use the MPI_Allgather () function to collect and store the results in each thread, and finally each thread gets a as the latest solution
if (id == 0)
{
for (i = 0; i < n; i++)
{
for (j = 0; j < n + 1; j++)
{
printf("%f\t", a[i][j]);
}
printf("\n");
}

/* for(i=0;i<n;i++)
{
printf("%d\t",picked[i]);
}
*/
double *x;
x = (double *)malloc(sizeof(double) * n);
for (i = (n - 1); i >= 0; i--) // There is also a small episode after this line Added ";", causing i to become - 1, causing the program to report Segmentation fault(11), this problem is often encountered under Linux, generally 2 points: 1. The memory occupied exceeds the system memory 2. In the loop, the array Out of bounds
{
//printf("\n n =%d,i = %d",n,i);
x[i] = a[picked[i]][n] / a[picked[i]][i];
printf("x[%d] = %f\n", i, x[i]);
for (j = 0; j < n; j++)
{
a[picked[j]][n] = a[picked[j]][n] - x[i] * a[picked[j]][i];
a[picked[j]][i] = 0;
}

}
}


MPI_Finalize();
return 0;
}


int NotIn(int id, int *picked)
{
int i;
for (i = 0; i < n; i++)
{
if (id == picked[i])
{
return 0;
}
}
return 1;
}
