// Copyright Utkin Andrey 2020

#pragma once

#include <mpi.h>

int MPI_MyReduce(const void* sendbuf, void* recvbuf, int count,
                 MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);
