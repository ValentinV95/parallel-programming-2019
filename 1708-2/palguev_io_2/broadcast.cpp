// Copyright 2019 Palguev Igor
#include "../../../modules/task_2/palguev_i_broadcast/broadcast.h"

void broadcast(void *buffer, int count, MPI_Datatype datatype, int root,
                 MPI_Comm comm) {
    int rank;
    MPI_Comm_rank(comm, &rank);
    int size;
    MPI_Comm_size(comm, &size);

    if (rank == root) {
        for (int i = 0; i < size; i++) {
            if (i != rank) {
                MPI_Send(buffer, count, datatype, i, 0, comm);
            }
        }
    } else {
        MPI_Recv(buffer, count, datatype, root, 0, comm, MPI_STATUS_IGNORE);
    }
}
