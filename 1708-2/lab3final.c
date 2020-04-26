/* mpirun -np 8 lab3final.c */
/* Enter the exact input then the mode and and rectangele */
/*ref 2009 book introduction to mpi by jun NI (hpc summer school) */
/* 0 1 512 1 */

#include <stdio.h>
#include "mpi.h"
#include <math.h>
int main(int argc, char** argv)
{
int my_rank;
int p;
float a = 0.0, b=1.0, h;
int n = 2048;
int mode=3; /* mode=1,2,3 rectangle, */
float local_a, local_b, local_h;
float local_integral, integral;
int source;
int dest = 0;
int tag = 0;
MPI_Status status;
/* function prototypes */
void Get_data(float* a_ptr, float* b_ptr,
int* n_ptr, int my_rank, int p, int *mode_ptr);
float rect(float local_a, float local_b, int local_n);
/* MPI starts */
MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD,
&my_rank);
MPI_Comm_size(MPI_COMM_WORLD, &p);
Get_data(&a, &b, &n, my_rank, p, &mode);
h = (b-a)/n;
local_h=h/p;
local_a = a + my_rank*local_h*n;
local_b = local_a + local_h*n;
switch(mode)
{
case(1):
local_integral = rect(local_a, local_b, n);
}
if(my_rank==0)
{
if (mode==1)
printf("Rectangle rule (0-point rule) is selected\n");
}
if (my_rank == 0)
{
integral = local_integral;
for (source = 1; source < p; source++)
{
MPI_Recv(&local_integral,1,MPI_FLOAT,source,tag,MPI_COMM_WORLD, &status);

integral += local_integral;}
}
else
{
printf("The intergal calculated from process %d is %f \n", my_rank,local_integral);
MPI_Send(&local_integral, 1, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
}
if (my_rank == 0)
{
printf("With n = %d, the total integral from %f to %f= %f\n",n, a,b,integral);
}
/* MPI finished */
MPI_Finalize();
}
/**********************************************
**********************/
/* Function Get_data
* Reads in the user input a, b, and n.
* Input parameters:
* 1. int my_rank: rank of current process* 2. int p: number of processes.
* Output parameters:
* 1. float* a_ptr: pointer to left endpoint a.
* 2. float* b_ptr: pointer to right endpoint b.
* 3. int* n_ptr: pointer to number of trapezoids.
3. int* mode_ptr: pointer to mode of rule of NewtonCotes methods
* Algorithm:
* 1. Process 0 prompts user for input and
* reads in the values.
* 2. Process 0 sends input values to other
* processes.
*/
void Get_data(
float* a_ptr /* out */,
float* b_ptr /* out */,
int* n_ptr /* out */,
int my_rank /* in */,
int p /* in */,
int* mode_ptr /* out */)
{
int source = 0; /* All local variables used by */
int dest; /* MPI_Send and MPI_Recv */
int tag;
MPI_Status status ;
if (my_rank == 0)
{
do
{
printf("Enter a, b, n(1024), and mode(1--rect):\n");
scanf_s("%f %f %d %d", a_ptr, b_ptr, n_ptr, mode_ptr);
}
 while (*mode_ptr<1 || *mode_ptr>3);
for (dest = 1; dest < p; dest++)
{
tag = 0;
MPI_Send(a_ptr, 1, MPI_FLOAT, dest, tag,MPI_COMM_WORLD);
tag = 1;
MPI_Send(b_ptr, 1, MPI_FLOAT, dest, tag,MPI_COMM_WORLD);
tag = 2;
MPI_Send(n_ptr, 1, MPI_INT, dest, tag,MPI_COMM_WORLD);
tag = 3;
MPI_Send(mode_ptr, 1, MPI_INT, dest, tag,MPI_COMM_WORLD);
}
}
else
{
tag = 0;
MPI_Recv(a_ptr, 1, MPI_FLOAT, source, tag,MPI_COMM_WORLD, &status);
tag = 1;
MPI_Recv(b_ptr, 1, MPI_FLOAT, source, tag,MPI_COMM_WORLD, &status);
tag = 2;
MPI_Recv(n_ptr, 1, MPI_INT, source, tag,MPI_COMM_WORLD, &status);
tag = 3;
MPI_Recv(mode_ptr, 1, MPI_INT, source, tag,MPI_COMM_WORLD, &status);
}
} /* Get_data */
float rect( float local_a, float local_b, int local_n)
{
float local_integral;
float x;
int i;
float local_h;
float f(float x);
local_h=(local_b-local_a)/local_n;
local_integral = f(local_a);
x = local_a;
for (i = 1; i <= local_n-1; i++)
{
x = x + local_h;
local_integral += f(x);
}
local_integral *=local_h;
return local_integral;
}
float f(float x)
{ return x*x; }
