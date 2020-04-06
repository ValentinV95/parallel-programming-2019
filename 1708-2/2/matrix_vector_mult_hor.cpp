// Copyright 2020 Strelzov Roman
#include "../../../modules/task_2/strelzov_r_matrix_vector_mult_hor/matrix_vector_mult_hor.h"
#include <mpi.h>
#include <iostream>
#include <random>
#include <vector>
#include <utility>
#include <ctime>

std::vector<int> matrix_vector_mult_parallel(const std::vector<int>& matr,
                                                const std::vector<int>& vect, int width, int height) {
    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<int> ans;
    std::vector<int> vec, mat;
    if (rank == 0) {
        vec = vect;
    } else {
        vec.resize(vect.size());
    }
    MPI_Bcast(vec.data(), vec.size(), MPI_INT, 0, MPI_COMM_WORLD);
    std::vector<int>begin(size);
    std::vector<int>size_on_proc(size);
    for (int i = 0; i < size; i++) {
        size_on_proc[i] = height/size;
        if (i < height % size)
            size_on_proc[i]++;
        size_on_proc[i] *= width;
        if (i) {
            begin[i] = begin[i-1] + size_on_proc[i];
        } else {
            begin[i] = 0;
        }
    }
    mat.resize(size_on_proc[rank]);
    std::vector<int>ans1;
    if (rank == 0) {
        ans1.resize(height);
    }
    MPI_Scatterv(matr.data(), size_on_proc.data(), begin.data(),
                    MPI_INT, mat.data(), size_on_proc[rank], MPI_INT, 0, MPI_COMM_WORLD);
    for (int i = 0; i < size; i++) {
        size_on_proc[i] /= width;
        if (i)
            begin[i] = begin[i-1] + size_on_proc[i];
    }
    ans.resize(size_on_proc[rank]);
    for (int i = 0; i < size_on_proc[rank]; i++) {
        ans[i] = 0;
        for (int j = 0; j < width; j++) {
            ans[i] += mat[i*width + j] * vec[j];
        }
    }
    MPI_Gatherv(ans.data(), size_on_proc[rank], MPI_INT, ans1.data(),
                    size_on_proc.data(), begin.data(), MPI_INT, 0, MPI_COMM_WORLD);
    return ans1;
}

std::vector<int> matrix_vector_mult_sequential(const std::vector<int>& mat,
                                                    const std::vector<int>& vec, int width, int height) {
    std::vector<int> ans(height);
    for (int i = 0; i < height; i++) {
        ans[i] = 0;
        for (int j = 0; j < width; j++) {
            ans[i] += mat[i*width + j] * vec[j];
        }
    }
    return ans;
}

std::vector<int> generate_vector(int size) {
    std::mt19937 mt(time(0));
    std::uniform_int_distribution<int> rand(-10, 10);
    std::vector<int> v;
    v.resize(size);
    for (int i = 0; i < size; i++)
        v[i] = rand(mt);
    return v;
}
