// Copyright Utkin Andrey 2020

#include <gtest/gtest.h>
#include <mpi.h>

#include <gtest-mpi-listener.hpp>
#include <string>

#include "./word_count.h"

TEST(String_Word_Count, Seq_Correct) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    std::string testString = getRandomString(20);

    ASSERT_EQ(wordCount(testString), 20);
  }
}

TEST(String_Word_Count, Parll_Correct) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::string testString("");
  if (rank == 0) {
    testString = getRandomString(30);
  }

  size_t words = wordCount_p(testString);

  if (rank == 0) {
    ASSERT_EQ(words, wordCount(testString));
  }
}

TEST(String_Word_Count, Parll_100000_words) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::string testString("");
  double estTime;
  if (rank == 0) {
    testString = getRandomString(100000);
    estTime = MPI_Wtime();
  }

  size_t words = wordCount_p(testString);

  if (rank == 0) {
    estTime = MPI_Wtime() - estTime;
    std::cout << "Estimated time parll: " << estTime << "s" << std::endl;

    estTime = MPI_Wtime();
    size_t wordsExp = wordCount(testString);
    estTime = MPI_Wtime() - estTime;
    std::cout << "Estimated time seq: " << estTime << "s" << std::endl;

    ASSERT_EQ(words, wordsExp);
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
