// Copyright 2020 Utkin Andrey

#include <gtest/gtest.h>
#include <mpi.h>

#include <gtest-mpi-listener.hpp>
#include <vector>

#include "./batcher_sort.h"

TEST(Batcher_Sort, Merge) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    std::vector<int> a{1, 3, 4, 7};
    std::vector<int> b{2, 5, 6, 8, 9};
    std::vector<int> buff;
    std::vector<int> exp{1, 2, 3, 4, 5, 6, 7, 8, 9};

    oddEvenMerge(a, b, buff);

    ASSERT_EQ(buff, exp);
  }
}

TEST(Batcher_Sort, Radix_Sort) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    std::vector<int> a = randomVector(1000);
    std::vector<int> exp(a);

    std::sort(exp.begin(), exp.end());

    radixSort(a);

    ASSERT_EQ(exp, a);
  }
}

TEST(Batcher_Sort, Parll_Sort_1000) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::vector<int> arr;

  if (rank == 0) {
    arr = randomVector(1000);
    std::vector<int> exp(arr);

    sortParll(arr);

    std::sort(exp.begin(), exp.end());

    ASSERT_EQ(exp, arr);
  } else {
    sortParll(arr);
  }
}

TEST(Batcher_Sort, Parll_Sort_100) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::vector<int> arr;

  if (rank == 0) {
    arr = randomVector(100);
    std::vector<int> exp(arr);

    sortParll(arr);

    std::sort(exp.begin(), exp.end());

    ASSERT_EQ(exp, arr);
  } else {
    sortParll(arr);
  }
}

TEST(Batcher_Sort, Parll_Sort_500) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::vector<int> arr;

  if (rank == 0) {
    double estTime;
    arr = randomVector(100000);
    std::vector<int> exp(arr);

    estTime = MPI_Wtime();
    sortParll(arr);
    estTime = MPI_Wtime() - estTime;
    std::cout << "Est time radix parll: " << estTime << "s" << std::endl;

    estTime = MPI_Wtime();
    std::sort(exp.begin(), exp.end());
    estTime = MPI_Wtime() - estTime;
    std::cout << "Est time std::sort: " << estTime << "s" << std::endl;

    ASSERT_EQ(exp, arr);
  } else {
    sortParll(arr);
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
