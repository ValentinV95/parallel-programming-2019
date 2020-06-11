#include <mpi.h>
#include <iostream>
#include <string>

void brcast(void *value, int len, MPI_Datatype dp, int root,
                 MPI_Comm cm, int rank, int size);

/**
 * @param ... root_process type(0, ..., 3) value count
 */
int main (int argc, char *argv[]) {
    int rank, size;
    MPI_Datatype types[4] = {MPI_INT, MPI_FLOAT, MPI_DOUBLE, MPI_CHAR};
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (argc != 5) {
        if (rank == 0) {
            std::cout << "Invalid agruments.";
        }
        MPI_Finalize();
        exit(0);
    }

    int root = atoi(argv[1]);
    int type = atoi(argv[2]);
    int count = atoi(argv[4]);

    switch (type)
    {
        case 0:
            int int_value;
            if (rank == root) {
                int_value = atoi(argv[3]);
            }
            brcast(&int_value, count, types[type], root, MPI_COMM_WORLD, rank, size);
            std::cout << "rank " << rank << ": " << int_value << std::endl;
            break;

        case 1:
            float float_value;
            if (rank == root) {
                float_value = std::stof(argv[3]);
            }
            brcast(&float_value, count, types[type], root, MPI_COMM_WORLD, rank, size);
            std::cout << "rank " << rank << ": " << float_value << std::endl;
            break;

        case 2:
            double double_value;
            if (rank == root) {
                double_value = std::stod(argv[3]);
            }
            brcast(&double_value, count, types[type], root, MPI_COMM_WORLD, rank, size);
            std::cout << "rank " << rank << ": " << double_value << std::endl;
            break;

        case 3:
            char char_value;
            if (rank == root) {
                char_value = argv[3][0];
            }
            brcast(&char_value, count, types[type], root, MPI_COMM_WORLD, rank, size);
            std::cout << "rank " << rank << ": " << char_value << std::endl;
            break;
            
        default:
            if (rank == 0)
                std::cout << "Invalid arguments.";
    }

    MPI_Finalize();
    return 0;
}

void brcast(void *value, int len, MPI_Datatype dp, int root,
                 MPI_Comm cm, int rank, int size) {
    if (rank == root) {
        int procces = 0;
        while (procces != size) {
            MPI_Send(value, len, dp, procces++, 0, cm);
        }
    }
    MPI_Barrier(cm);

    MPI_Request req;
    MPI_Status stat;
    MPI_Irecv(value, len, dp, root, 0, cm, &req);
    MPI_Wait(&req, &stat);
}
