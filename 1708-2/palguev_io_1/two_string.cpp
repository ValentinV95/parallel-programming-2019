#include <mpi.h>
#include <iostream>
#include "./two_string.h"

int diff_count_parallel(const char* str_1,
                        const char* str_2) {
    if (strlen(str_1) != strlen(str_2)) {
        throw "ERR_LEN";
    }

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (size > (int)strlen(str_1)) {
        return diff_count_sequential(str_1, str_2, 0, strlen(str_1));
    }

    int delta = strlen(str_1) / size;
    int start = rank * delta;
    int end = start + delta;
    if (rank == size - 1)
        end += strlen(str_1) % size;

    int res = 0;
    int local_res = diff_count_sequential(str_1, str_2, start, end);
    MPI_Reduce(&local_res, &res, 1, MPI_CHAR, MPI_SUM, 0, MPI_COMM_WORLD);
    return res;
}

int diff_count_sequential(const char* str_1, const char* str_2, int start, int end) {
    int res = 0;

    for (int i = start; i < end; i++) {
        if (str_1[i] != str_2[i])
            res++;
    }

    return res;
}
