// Copyright Utkin Andrey 2020

#include "./reduce.h"

#include <mpi.h>

#include <algorithm>
#include <iostream>

template <class T>
void reduce(T& sendelem, T& recvelem, MPI_Op op) {
  switch (op) {
    case MPI_MAX:
      recvelem = std::max(sendelem, recvelem);
      break;
    case MPI_MIN:
      recvelem = std::min(sendelem, recvelem);
      break;
    case MPI_SUM:
      recvelem += sendelem;
      break;
    case MPI_PROD:
      recvelem *= sendelem;
      break;
  }
}

void cast_reduce(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype,
                 MPI_Op op) {
  for (int i = 0; i < count; i++) {
    switch (datatype) {
      case MPI_INT:
        reduce(static_cast<int*>(sendbuf)[i], static_cast<int*>(recvbuf)[i],
               op);
        break;
      case MPI_DOUBLE:
        reduce(static_cast<double*>(sendbuf)[i],
               static_cast<double*>(recvbuf)[i], op);
        break;
      case MPI_FLOAT:
        reduce(static_cast<float*>(sendbuf)[i], static_cast<float*>(recvbuf)[i],
               op);
        break;
    }
  }
}

int MPI_MyReduce(const void* sendbuf, void* recvbuf, int count,
                 MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm) {
  if (count <= 0) {
    return MPI_ERR_COUNT;
  }

  if (sendbuf == nullptr) {
    return MPI_ERR_BUFFER;
  }

  if (datatype != MPI_INT && datatype != MPI_DOUBLE && datatype != MPI_FLOAT) {
    return MPI_ERR_TYPE;
  }

  if (op != MPI_MAX && op != MPI_MIN && op != MPI_SUM && op != MPI_PROD) {
    return MPI_ERR_OP;
  }

  int size;
  int rank;
  if (MPI_Comm_size(comm, &size) == MPI_ERR_COMM ||
      MPI_Comm_rank(comm, &rank) == MPI_ERR_COMM) {
    return MPI_ERR_COMM;
  }

  if (rank == root && recvbuf == nullptr) {
    return MPI_ERR_BUFFER;
  }

  if (rank != root) {
    MPI_Send(sendbuf, count, datatype, root, 0, MPI_COMM_WORLD);

    return MPI_SUCCESS;
  } else {
    void* buf;
    // Сделать reduce в свой буфер если root
    switch (datatype) {
      case MPI_INT:
        buf = std::malloc(count * sizeof(int));
        std::memcpy(buf, sendbuf, count * sizeof(int));
        break;
      case MPI_DOUBLE:
        buf = std::malloc(count * sizeof(double));
        std::memcpy(buf, sendbuf, count * sizeof(double));
        break;
      case MPI_FLOAT:
        buf = std::malloc(count * sizeof(float));
        std::memcpy(buf, sendbuf, count * sizeof(float));
        break;
    }
    cast_reduce(buf, recvbuf, count, datatype, op);
    // Принять данные от других потоков и сделать reduce
    for (int proc = 0; proc < size; proc++) {
      if (proc != rank) {
        int status = MPI_Recv(buf, count, datatype, MPI_ANY_SOURCE, MPI_ANY_TAG,
                              MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (status != MPI_SUCCESS) {
          return status;
        }

        cast_reduce(buf, recvbuf, count, datatype, op);
      }
    }

    return MPI_SUCCESS;
  }
}
