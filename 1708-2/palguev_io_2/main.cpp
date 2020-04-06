#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include "./broadcast.h"

TEST(Broadcast_MPI, Test_On_Int) {
    int test_int;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        test_int = 15;
    } 
    
    broadcast(&test_int, 1, MPI_INT, 0, MPI_COMM_WORLD);

    EXPECT_EQ(test_int, 15);
}

TEST(Broadcast_MPI, Test_On_Float) {
    float test_float;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        test_float = 15.0;   
    }

    broadcast(&test_float, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    EXPECT_EQ(test_float, 15.0);
}

TEST(Broadcast_MPI, Test_On_Double) {
    double test_double;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        test_double = 15.032;
    }
    
    broadcast(&test_double, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    EXPECT_EQ(test_double, 15.032);
}

TEST(Broadcast_MPI, Test_On_Char) {
    char test_char;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        test_char = 'a';
    }
    
    broadcast(&test_char, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
    
    EXPECT_EQ(test_char, 'a');
}

TEST(Broadcast_MPI, Test_On_Vector) {
    std::vector<int> test_vec(5);
    std::vector<int> res = {1, 2, 3, 4, 5};
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        test_vec = {1, 2, 3, 4, 5};
    }
    
    broadcast(&test_vec[0], 5, MPI_INT, 0, MPI_COMM_WORLD);
    
    EXPECT_EQ(test_vec, res);
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
