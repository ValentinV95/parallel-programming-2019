#pragma once

#include <mpi.h>

void broadcast(void *buffer, int count, MPI_Datatype datatype, int root,
                 MPI_Comm comm);
