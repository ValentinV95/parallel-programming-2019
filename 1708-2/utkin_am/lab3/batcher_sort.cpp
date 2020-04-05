// Copyright 2020 Utkin Andrey

#include "./batcher_sort.h"

#include <mpi.h>

#include <random>
#include <vector>

uint8_t getByte(const int& value, const uint8_t index) {
  return (value >> (index << 3)) & 0xff;
}

void genHistograms(std::vector<int>& hist, std::vector<int>& a,
                   uint8_t byteInd) {
  for (size_t i = 0; i < a.size(); i++) {
    if (byteInd != 3) {
      uint8_t byte = getByte(a[i], byteInd);
      hist[byte]++;
    } else {
      char byte = static_cast<char>(getByte(a[i], 3));
      hist[byte + 128]++;
    }
  }
}

void genOffsets(std::vector<int>& offsets, const std::vector<int>& hist) {
  offsets[0] = 0;
  for (int i = 1; i < 256; i++) {
    offsets[i] = hist[i - 1] + offsets[i - 1];
  }
}

void oddEvenMerge(const std::vector<int>& a, const std::vector<int>& b,
                  std::vector<int>& buff) {
  auto iterA = a.begin();
  auto iterB = b.begin();

  while (iterB != b.end() && iterA != a.end()) {
    if (*iterA <= *iterB) {
      buff.push_back(*iterA);
      iterA++;
    } else {
      buff.push_back(*iterB);
      iterB++;
    }
  }

  if (iterA == a.end()) {
    for (auto i = iterB; i != b.end(); i++) {
      buff.push_back(*i);
    }
  }

  if (iterB == b.end()) {
    for (auto i = iterA; i != a.end(); i++) {
      buff.push_back(*i);
    }
  }
}

std::vector<int> randomVector(const size_t size) {
  std::mt19937 gen;
  std::random_device rand_device;
  gen.seed(rand_device());

  std::uniform_int_distribution<int> dist(0, 1000);

  std::vector<int> data(size);

  std::generate(data.begin(), data.end(), [&]() { return dist(gen); });

  return data;
}

void radixSort(std::vector<int>& a) {
  if (a.size() <= 1) {
    return;
  }

  if (a.size() == 2) {
    if (a[0] > a[1]) {
      std::swap(a[0], a[1]);
    }

    return;
  }

  std::vector<int> tmp(a.size());
  std::vector<int> hist(256, 0);
  std::vector<int> offsets(256, 0);

  for (int i = 0; i <= 3; i++) {
    genHistograms(hist, a, i);

    genOffsets(offsets, hist);

    for (size_t j = 0; j < a.size(); j++) {
      if (i != 3) {
        uint8_t byte = getByte(a[j], i);

        tmp[offsets[byte]] = a[j];
        offsets[byte]++;
      } else {
        char byte = static_cast<char>(getByte(a[j], 3));

        tmp[offsets[byte + 128]] = a[j];
        offsets[byte + 128]++;
      }
    }

    std::swap(a, tmp);

    hist.clear();
    hist.resize(256, 0);

    offsets.clear();
    offsets.resize(256, 0);
  }
}

void sortParll(std::vector<int>& a) {
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size == 1) {
    radixSort(a);
    return;
  }

  int delta, rem = 0;

  if (rank == 0) {
    delta = a.size() / size;
    rem = a.size() % size;
  }

  MPI_Bcast(&delta, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (delta <= 1) {
    if (rank == 0) {
      radixSort(a);
      return;
    } else {
      return;
    }
  }

  // рассылка массива

  std::vector<int> localArr(delta);

  if (rank == 0) {
    MPI_Scatter(&a[rem], delta, MPI_INT, &localArr[0], delta, MPI_INT, 0,
                MPI_COMM_WORLD);
  } else {
    MPI_Scatter(MPI_IN_PLACE, 0, MPI_INT, &localArr[0], delta, MPI_INT, 0,
                MPI_COMM_WORLD);
  }

  if (rem != 0) {
    localArr.insert(localArr.end(), a.begin(), a.begin() + rem);
  }

  // сортировка и сборка

  if (rank == 0) {
    a = localArr;
    radixSort(a);

    std::vector<int> b(delta);
    std::vector<int> buff;

    MPI_Recv(&b[0], delta, MPI_INT, rank + 1, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);

    oddEvenMerge(a, b, buff);
    a = buff;

    b.clear();
    b.resize(delta * 2);
    buff.clear();

    // собираем отсортированный кусок массива с соседнего потока, потом через
    // один

    for (int proc = 2; proc < size - 1; proc += 2) {
      MPI_Recv(&b[0], delta * 2, MPI_INT, proc, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);

      oddEvenMerge(a, b, buff);
      a = buff;

      buff.clear();
    }

  } else {
    radixSort(localArr);

    // собираем отсортированный кусок массива с соседнего потока и отправляем на
    // 0

    if (rank % 2 != 0) {
      MPI_Send(&localArr[0], delta, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
    } else {
      std::vector<int> b(delta);
      std::vector<int> buff;

      MPI_Recv(&b[0], delta, MPI_INT, rank + 1, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);

      oddEvenMerge(localArr, b, buff);

      MPI_Send(&buff[0], delta * 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
  }
}
