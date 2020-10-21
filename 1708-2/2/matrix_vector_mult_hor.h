// Copyright 2020 Strelzov Roman
#ifndef MODULES_TASK_2_STRELZOV_R_MATRIX_VECTOR_MULT_HOR_MATRIX_VECTOR_MULT_HOR_H_
#define MODULES_TASK_2_STRELZOV_R_MATRIX_VECTOR_MULT_HOR_MATRIX_VECTOR_MULT_HOR_H_
#include <vector>

std::vector<int> matrix_vector_mult_parallel(const std::vector<int>& matr,
                                                const std::vector<int>& vect, int width, int height);
std::vector<int> matrix_vector_mult_sequential(const std::vector<int>& mat,
                                                const std::vector<int>& vec, int width, int height);
std::vector<int> generate_vector(int size);

#endif  // MODULES_TASK_2_STRELZOV_R_MATRIX_VECTOR_MULT_HOR_MATRIX_VECTOR_MULT_HOR_H_
