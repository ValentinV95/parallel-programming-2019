#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>



int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);

	int world_rank, nranks;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nranks);

	const int MAX_NUMBERS = 100;
	int numbers[MAX_NUMBERS];
	for (int i = 0; i < MAX_NUMBERS; i++) {
		numbers[i] = i;
	}
	double roots[MAX_NUMBERS];
	int number_amount;
	if (world_rank == 0) { // Producer
		srand(time(NULL));
		number_amount = (rand() / (float)RAND_MAX) * MAX_NUMBERS;
		printf("[0] Distributing %i numbers in total.\n", number_amount); //Распределение номеров

		for (int nextnum = 1; nextnum <= number_amount; ++nextnum) {
			// Ждет пока работающий станет доступным
			MPI_Status status;
			double root = 0;
			MPI_Recv(&root, 1, MPI_DOUBLE, MPI_ANY_SOURCE,
				MPI_ANY_TAG, MPI_COMM_WORLD, &status);

			// Если корень был вычислен
			if (status.MPI_TAG > 0) {
				roots[status.MPI_TAG] = root;
			}

			printf("[0] Distributing %i to rank %i.\n", numbers[nextnum], status.MPI_SOURCE); //Распределение по рангу
			MPI_Send(&(numbers[nextnum]), 1, MPI_INT, status.MPI_SOURCE,
				nextnum, MPI_COMM_WORLD);
		}

		// Сигнал завершения каждому рангу, когда они отправляют свою последнюю работу
		int num_terminated = 0;
		for (int num_terminated = 0; num_terminated < nranks - 1; num_terminated++) {
			// Ждет пока работающий станет доступным
			MPI_Status status;
			double root = 0;
			MPI_Recv(&root, 1, MPI_DOUBLE, MPI_ANY_SOURCE,
				MPI_ANY_TAG, MPI_COMM_WORLD, &status);

			// Если корень был вычислен
			if (status.MPI_TAG > 0) {
				roots[status.MPI_TAG] = root;
			}

			printf("[0] Terminating rank %i.\n", status.MPI_SOURCE); //Завершающий ранг
			// Отправить сигнал завершения (tag = 0)
			MPI_Send(&num_terminated, 1, MPI_INT, status.MPI_SOURCE,
				0, MPI_COMM_WORLD);
		}
	}
	else { // Consumer
		   // Объявить себя Producer
		double root = 0;
		MPI_Send(&root, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		bool terminated = false;

		do {
			// Wait for a job
			int num = 0;
			MPI_Status status;
			MPI_Recv(&num, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			//printf("[%i] Received job: %i [tag = %i].\n", world_rank, num, status.MPI_TAG);
			if (status.MPI_TAG == 0) {
				terminated = true;
				printf("[%i] Terminated.\n", world_rank); //Отменено
			}
			else {
				root = sqrt(num);
				printf("[%i] Submitting result: sqrt(%i) = %f.\n", world_rank, num, root); //Отправка результата
				MPI_Send(&root, 1, MPI_DOUBLE, 0, status.MPI_TAG, MPI_COMM_WORLD);
			}
		} while (!terminated);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if (world_rank == 0) {
		printf("\n\nResults:\n");
		for (int i = 1; i <= number_amount; i++) {
			printf("sqrt(%i) = %f\n", numbers[i], roots[i]);
		}
	}

	MPI_Finalize();
}