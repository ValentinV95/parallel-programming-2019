// Copyright Utkin Andrey 2020

#include <gtest/gtest.h>
#include <mpi.h>

#include <gtest-mpi-listener.hpp>
#include <vector>

#include "./reduce.h"

TEST(Reduce, Reduce_Correct) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::vector<int> testVector{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  if (rank == 0) {
    std::vector<int> sum(testVector.size());
    std::vector<int> expSum(testVector.size());
    double time = MPI_Wtime();

    MPI_MyReduce(&testVector[0], &sum[0], testVector.size(), MPI_INT, MPI_SUM,
                 0, MPI_COMM_WORLD);
    time = MPI_Wtime() - time;
    std::cout << "Estimated time MyReduce: " << time << "s" << std::endl;

    time = MPI_Wtime();
    MPI_Reduce(&testVector[0], &expSum[0], testVector.size(), MPI_INT, MPI_SUM,
               0, MPI_COMM_WORLD);
    time = MPI_Wtime() - time;
    std::cout << "Estimated time MPIReduce: " << time << "s" << std::endl;

    ASSERT_EQ(sum, expSum);
  } else {
    MPI_MyReduce(&testVector[0], MPI_IN_PLACE, testVector.size(), MPI_INT,
                 MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&testVector[0], MPI_IN_PLACE, testVector.size(), MPI_INT,
               MPI_SUM, 0, MPI_COMM_WORLD);
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
