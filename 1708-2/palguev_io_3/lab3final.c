//mpiexec - n 4 . /(name of the fil) 4
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

#define DEBUG 1
void MatrixMultiplyAgg(int n, double *a, double *b, double *c);

int main(int argc, char *argv[])
{
    int i, j;
    int n, nlocal;
    double *a, *b, *c;
    int npes, dims[2], periods[2];
    int myrank, my2drank, mycoords[2];
    int shiftsource, shiftdest;
    int rightrank, leftrank, downrank, uprank;
    MPI_Status status;
    MPI_Comm comm_2d;


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &npes);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    double start = MPI_Wtime();

    if (myrank == 0)
        printf("%d processors\n", npes);

    if (argc != 2)
    {
        if (myrank == 0)
        printf("Usage: %s <the dimension of the matrix>\n", argv[0]);
        MPI_Finalize();
        exit(0);
    }


    dims[0] = sqrt(npes);
    dims[1] = npes / dims[0];
    if (dims[0] != dims[1])
    {
        if (myrank == 0) {
            printf("The number of processes must be perfect square.\n");
        }
        MPI_Finalize();
        exit(0);
    }

    // logical array of size ndims specifying whether the grid is
    // periodic (true) or not (false) in each dimension
    periods[0] = periods[1] = 1;

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &comm_2d);
    MPI_Comm_rank(comm_2d, &my2drank);
    MPI_Cart_coords(comm_2d, my2drank, 2, mycoords);

    n = atoi(argv[1]); // n
    nlocal = n / dims[0];
    a = (double*)malloc(nlocal*nlocal * sizeof(double));
    b = (double *)malloc(nlocal*nlocal * sizeof(double));
    c = (double *)malloc(nlocal*nlocal * sizeof(double));

    if (DEBUG)
        printf("%d: init matrix\n", myrank);


    for (i = 0; i < nlocal*nlocal; i++) {
        a[i] = myrank;
        b[i] = myrank;
        c[i] = 0.0;
    }

    if (DEBUG)
        printf("%d: done initing matrix\n", myrank);

    MPI_Barrier(MPI_COMM_WORLD);


    if (DEBUG) {
        printf("%d: x:%d,y:%d\n", myrank, mycoords[0], mycoords[1]);
    }

    MPI_Cart_shift(comm_2d, 0, -mycoords[1], &shiftsource, &shiftdest);
    if (DEBUG) {
        printf("%d: dest:%d,source:%d\n", myrank, shiftdest, shiftsource);
    }
    MPI_Sendrecv_replace(a, nlocal*nlocal, MPI_DOUBLE, shiftdest, 1, shiftsource, 1, comm_2d, &status);

    MPI_Barrier(comm_2d);

    MPI_Cart_shift(comm_2d, 1, -mycoords[0], &shiftsource, &shiftdest);
    if (DEBUG) {
        printf("%d: dest:%d,source:%d\n", myrank, shiftdest, shiftsource);
    }
    MPI_Sendrecv_replace(b, nlocal*nlocal, MPI_DOUBLE, shiftdest, 1, shiftsource, 1, comm_2d, &status);

    if (DEBUG)
    {
        printf("%d: ready to start calculating\n", myrank);
    }

    MPI_Barrier(comm_2d);


    MPI_Cart_shift(comm_2d, 0, -1, &rightrank, &leftrank);
    MPI_Cart_shift(comm_2d, 1, -1, &downrank, &uprank);

    if (DEBUG)
    {
        printf("%d: right:%d, left:%d, up:%d, down:%d\n",
                myrank, rightrank, leftrank, uprank, downrank);
    }

    for (i = 0; i < dims[0]; i++)
    {
        MPI_Barrier(comm_2d);
        MatrixMultiplyAgg(nlocal, a, b, c);
        MPI_Sendrecv_replace(a, nlocal*nlocal,
        MPI_DOUBLE, leftrank, 1, rightrank, 1, comm_2d, &status);
        MPI_Sendrecv_replace(b, nlocal*nlocal, MPI_DOUBLE, uprank, 1, downrank, 1, comm_2d, &status);
    }

    MPI_Barrier(comm_2d);


    MPI_Cart_shift(comm_2d, 0, -mycoords[1], &shiftsource, &shiftdest);
    MPI_Sendrecv_replace(a, nlocal*nlocal, MPI_DOUBLE, shiftdest, 1, shiftsource, 1, comm_2d, &status);

    MPI_Barrier(comm_2d);

    MPI_Cart_shift(comm_2d, 1, -mycoords[0], &shiftsource, &shiftdest);
    MPI_Sendrecv_replace(b, nlocal*nlocal, MPI_DOUBLE, shiftdest, 1, shiftsource, 1, comm_2d, &status);

    MPI_Barrier(comm_2d);

    MPI_Comm_free(&comm_2d);

    if (DEBUG)
        printf("%d: finish calculating\n", myrank);

    MPI_Barrier(MPI_COMM_WORLD);

    int rank = 0;
    while (rank < npes) {
        if (myrank == rank) {
            printf("my rank: %d\n", myrank);
            //printf("x:%d,y:%d\n", mycoords[0], mycoords[1]);
            puts("Random Matrix A");
            for (i = 0; i < nlocal; i++)
            {
                for (j = 0; j < nlocal; j++)
                    printf("%6.3f ", a[i*nlocal + j]);
                printf("\n");
            }
            puts("Random Matrix B");
            for (i = 0; i < nlocal; i++)
            {
                for (j = 0; j < nlocal; j++)
                    printf("%6.3f ", b[i*nlocal + j]);
                printf("\n");
            }
            puts("Matrix C = A*B");
            for (i = 0; i < nlocal; i++)
            {
                for (j = 0; j < nlocal; j++)
                    printf("%6.3f ", c[i*nlocal + j]);
                printf("\n");
            }
            free(a);
            free(b);
            free(c);
        }
        rank++;
        MPI_Barrier(MPI_COMM_WORLD);
    }
    double end = MPI_Wtime() - start;
    if (myrank == 0) {
        printf("PLL: %6.3f", end);
    }

    MPI_Finalize();
    return 0;
}

void MatrixMultiplyAgg(int n, double *a, double *b, double *c)
{
    int i, j, k;
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            for (k = 0; k < n; k++)
                c[i*n + j] += a[i*n + k] * b[k*n + j];
}
