#include "pch.h"
#include <stdio.h>
#include <iostream>
#include "mpi.h"

using namespace std;

bool check_point = false;

void counter(int Operations, int proc_N, int send, int receiv, int* arr_proc)
{
	int s = 0;
	if (check_point)
	{
		for (int i = send; i <= receiv; i++)
		{
			arr_proc[s] = i;
			s++;
		}
	}
	else
	{
		for (int i = send; i >= receiv; i--)
		{
			arr_proc[s] = i;
			s++;
		}
	}
}

int main(int argc, char* argv[])
{
	int send = atoi(argv[1]), proc_N, receiv = atoi(argv[2]), current_rank, op_val = 0;
	double time_start, time_end;
	int message = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_N);
	MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
	if (send == proc_N)
	{
		if (current_rank == 0)
		{
			cout << "send-processor_dont_be_a_receiv_processor_in_same_time!" << endl;
		}
		MPI_Finalize();
		return 0;
	}
	if (send > proc_N)
	{
		if (current_rank == 0)
		{
			cout << "send-processor_must_be_less_than_total_processors!" << endl;
		}
		MPI_Finalize();
		return 0;
	}
	if (receiv > proc_N)
	{
		if (current_rank == 0)
		{
			cout << "receiv-processor_must_be_less_than_total_processors!" << endl;
		}
		MPI_Finalize();
		return 0;
	}
	if (send == receiv)
	{
		if (current_rank == 0)
		{
			cout << "send-processor_equal_receiv-processor!" << endl;
		}
		MPI_Finalize();
		return 0;
	}
	else
	{
		if (current_rank == send)
		{
			cout << "\ntotal_proccesors: " << proc_N << "\nsend-processor: " << send << "\nreceiv-processor: " << receiv << "\n\n";
		}
		if (send < receiv)
		{
			check_point = true;
			op_val = receiv - send + 1;
		}
		else
		{
			check_point = false;
			op_val = send - receiv + 1;
		}
		int* arr_proc = new int[op_val];
		counter(op_val, proc_N, send, receiv, arr_proc);

		for (int i = 0; i < op_val; i++)
		{
			if (arr_proc[i] == current_rank && current_rank == send)
			{
				message = 200;
				MPI_Send(&message, 1, MPI_INT, arr_proc[i + 1], 500, MPI_COMM_WORLD);
				cout << "\n\nprocess: " << current_rank << "\nsending_to: " << arr_proc[i + 1];
				break;
			}
			if (arr_proc[i] == current_rank && current_rank == receiv)
			{
				MPI_Recv(&message, 1, MPI_INT, arr_proc[i - 1], MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				cout << "\n\nprocess: " << current_rank << "\nreceiving_from: " << arr_proc[op_val - 2] << "\nmessage: " << message;
				break;
			}
			if (arr_proc[i] == current_rank && current_rank != send && current_rank != receiv)
			{
				MPI_Recv(&message, 1, MPI_INT, arr_proc[i - 1], MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Send(&message, 1, MPI_INT, arr_proc[i + 1], 500, MPI_COMM_WORLD);
				cout << "\n\nprocess: " << current_rank << "\nreceiving_from: " << arr_proc[i - 1] << "\nsending_to: " << arr_proc[i + 1];
				break;
			}
		}
		MPI_Finalize();
		return 0;
	}

}