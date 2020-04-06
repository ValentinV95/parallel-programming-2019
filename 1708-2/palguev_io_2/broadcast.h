// Copyright 2019 Palguev Igor
#ifndef MODULES_TASK_2_PALGUEV_I_BROADCAST_BROADCAST_H
#define MODULES_TASK_2_PALGUEV_I_BROADCAST_BROADCAST_H

#include <mpi.h>

void broadcast(void *buffer, int count, MPI_Datatype datatype, int root,
                 MPI_Comm comm);

#endif  // MODULES_TASK_2_PALGUEV_I_BROADCAST_BROADCAST_H
