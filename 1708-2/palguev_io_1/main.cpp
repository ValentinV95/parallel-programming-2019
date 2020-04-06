// Copyright 2019 Sadikov Artem
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include "./two_string.h"
#include <iostream>

TEST(Two_String_MPI, Test_On_Min_String) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    char test_str_1[] = "abc";
    char test_str_2[] = "cba";
    

    int res = diff_count_parallel(test_str_1, test_str_2);

    if (rank == 0) {
        EXPECT_EQ(res, 2);
    }
}

TEST(Two_String_MPI, Test_On_Equal_String) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    char test_str_1[] = "abc";
    char test_str_2[] = "abc";
    

    int res = diff_count_parallel(test_str_1, test_str_2);

    if (rank == 0) {
        EXPECT_EQ(res, 0);
    }
}

TEST(Two_String_MPI, Test_On_Second_String) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    char test_str_1[] = "abcddddd";
    char test_str_2[] = "cbaaaaaa";
    

    int res = diff_count_parallel(test_str_1, test_str_2);

    if (rank == 0) {
        EXPECT_EQ(res, 7);
    }
}

TEST(Two_String_MPI, Test_On_Throw) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    char test_str_1[] = "abcds";
    char test_str_2[] = "cba";

    ASSERT_ANY_THROW(diff_count_parallel(test_str_1, test_str_2));
}

TEST(Two_String_MPI, Test_On_Third_String) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    char test_str_1[] = "abcewewqq";
    char test_str_2[] = "cbaddqwss";
    

    int res = diff_count_parallel(test_str_1, test_str_2);

    if (rank == 0) {
        EXPECT_EQ(res, 7);
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

