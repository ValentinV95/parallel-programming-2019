// Copyright 2020 Strelzov Roman
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <mpi.h>
#include <vector>
#include "./matrix_vector_mult_hor.h"

TEST(Matrix_Vector_Mult, Test_Seq_Mult) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int width = 3, height = 3;
    std::vector<int> mat = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> vec = {1, 2, 3};
    if (rank == 0) {
        std::vector<int> ans = matrix_vector_mult_sequential(mat, vec, width, height);
        std::vector<int> ans_expect = {14, 32, 50};
        EXPECT_EQ(ans, ans_expect);
    }
}

TEST(Matrix_Vector_Mult, Test_Paral_Mult_On_Small_Size) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int width = 5, height = 3;
    std::vector<int> mat = generate_vector(width*height);
    std::vector<int> vec = generate_vector(width);
    std::vector<int> ans = matrix_vector_mult_parallel(mat, vec, width, height);
    if (rank == 0) {
        std::vector<int> ans_seq = matrix_vector_mult_sequential(mat, vec, width, height);
        EXPECT_EQ(ans, ans_seq);
    }
}

TEST(Matrix_Vector_Mult, Test_Paral_Mult_On_Very_Small_Size) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int width = 1, height = 1;
    std::vector<int> mat = generate_vector(width*height);
    std::vector<int> vec = generate_vector(width);
    std::vector<int> ans = matrix_vector_mult_parallel(mat, vec, width, height);
    if (rank == 0) {
        std::vector<int> ans_seq = matrix_vector_mult_sequential(mat, vec, width, height);
        EXPECT_EQ(ans, ans_seq);
    }
}

TEST(Matrix_Vector_Mult, Test_Paral_Mult_On_Big_Size) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int width = 1000, height = 1000;
    std::vector<int> mat = generate_vector(width*height);
    std::vector<int> vec = generate_vector(width);
    std::vector<int> ans = matrix_vector_mult_parallel(mat, vec, width, height);
    if (rank == 0) {
        std::vector<int> ans_seq = matrix_vector_mult_sequential(mat, vec, width, height);
        EXPECT_EQ(ans, ans_seq);
    }
}

TEST(Matrix_Vector_Mult, Test_Paral_Mult_Small_Height) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int width = 500, height = 1;
    std::vector<int> mat = generate_vector(width*height);
    std::vector<int> vec = generate_vector(width);
    std::vector<int> ans = matrix_vector_mult_parallel(mat, vec, width, height);
    if (rank == 0) {
        std::vector<int> ans_seq = matrix_vector_mult_sequential(mat, vec, width, height);
        EXPECT_EQ(ans, ans_seq);
    }
}

TEST(Matrix_Vector_Mult, Test_Paral_Mult_Small_Width) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int width = 1, height = 500;
    std::vector<int> mat = generate_vector(width*height);
    std::vector<int> vec = generate_vector(width);
    std::vector<int> ans = matrix_vector_mult_parallel(mat, vec, width, height);
    if (rank == 0) {
        std::vector<int> ans_seq = matrix_vector_mult_sequential(mat, vec, width, height);
        EXPECT_EQ(ans, ans_seq);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    MPI_Init(&argc, &argv);

    ::testing::AddGlobalTestEnvironment(new GTestMPIListener::MPIEnvironment);
    ::testing::TestEventListeners& listeners =
        ::testing::UnitTest::GetInstance()->listeners();

    listeners.Release(listeners.default_result_printer());
    listeners.Release(listeners.default_xml_generator());

    listeners.Append(new GTestMPIListener::MPIMinimalistPrinter);
    return RUN_ALL_TESTS();
}
