// Copyright Utkin Andrey 2020

#include "./word_count.h"

#include <mpi.h>

#include <algorithm>
#include <random>
#include <string>
#include <vector>

std::string getRandomString(const int words) {
  if (words < 1) {
    return std::string("");
  }

  std::mt19937 gen;
  std::random_device rand_device;
  gen.seed(rand_device());

  std::vector<std::string> wordWault = {
      "Lorem",      "ipsum", "dolor",  "sit", "amet",    "consectetur",
      "adipiscing", "elit",  "sed",    "do",  "eiusmod", "tempor",
      "incididunt", "ut",    "labore", "et",  "dolore",  "magna",
      "aliqua",     "in",    "culpa",  "qui", "officia", "deserunt",
      "mollit",     "anim",  "id",     "est", "laborum"};
  std::uniform_int_distribution<int> dist(0, 28);

  std::string result;
  result.append("Begin ");
  for (size_t i = 0; i < words - 2; i++) {
    result.append(wordWault.at(dist(gen)));
    result.append(" ");
  }
  result.append("end.");

  return result;
}

size_t wordCount(const std::string& string) {
  if (string.empty()) {
    return -1;
  }

  // кол-во слов в строке = кол-во пробелов + 1
  return std::count(string.begin(), string.end(), ' ') + 1;
}

size_t wordCount_p(std::string& string) {
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size == 1) {
    return wordCount(string);
  }

  int delta, rem;
  if (rank == 0) {
    delta = string.size() / size;
    rem = string.size() % size;
  }

  MPI_Bcast(&delta, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (delta < 2) {
    if (rank == 0) {
      return wordCount(string);
    } else {
      return -1;
    }
  }
  // Рассылка исходной строки по потокам(остаток на 0)
  if (rank == 0) {
    MPI_Scatter(&string[rem], delta, MPI_CHAR, MPI_IN_PLACE, 0, MPI_CHAR, 0,
                MPI_COMM_WORLD);
  } else {
    string.resize(delta);
    MPI_Scatter(MPI_IN_PLACE, 0, MPI_CHAR, &string[0], delta, MPI_CHAR, 0,
                MPI_COMM_WORLD);
  }
  // Подсчет кол-ва слов(пробелов) в локальной строке
  size_t localWordCount;

  if (rank == 0) {
    localWordCount =
        std::count(string.begin(), string.begin() + delta + rem, ' ');
  } else {
    localWordCount = std::count(string.begin(), string.end(), ' ');
  }

  MPI_Barrier(MPI_COMM_WORLD);
  // Сбор данных
  if (rank == 0) {
    size_t wordCount = 0;

    MPI_Reduce(&localWordCount, &wordCount, 1, MPI_INT, MPI_SUM, 0,
               MPI_COMM_WORLD);

    return wordCount + 1;
  } else {
    MPI_Reduce(&localWordCount, MPI_IN_PLACE, 1, MPI_INT, MPI_SUM, 0,
               MPI_COMM_WORLD);

    return -1;
  }
}
