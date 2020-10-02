#include "mpi.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
    int Proc_N, Writers_Value = 0, Last_Reader = 0, Current_Rank;
    double W_Time_Start, W_Time_End;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &Proc_N);
    MPI_Comm_rank(MPI_COMM_WORLD, &Current_Rank);
    if (Proc_N == 1)
    {
        if (Current_Rank == 0)
        {
            cout << "\nInvalid input!\nThere is only one writer!\nValue of process must be greather than one.\n\n";
            MPI_Finalize();
            return 0;
        }
    }
    Writers_Value = Proc_N / 2; 							//Value of writer-reader pack
    if (Writers_Value == 0) Writers_Value++;
    int* Massive_Books_Sender = new int[Writers_Value];
    int* Massive_Books_Receiver = new int[Writers_Value];
    if (Proc_N % 2 != 0 && Writers_Value != 1)				//If value of Writers greather than Readers
    {
        Last_Reader = Proc_N % 2;
    }
    if (Current_Rank == 0)
    {
        cout << "\n\t\tWriters and readers.\n\t\tEgorov V. 381708-2.\n\n";
        cout << "\n\tTotal process = " << Proc_N << "\n\n\tTotal writers = " << Writers_Value << "\n\tTotal readers = " << Writers_Value + Last_Reader << "\n\n";
        int Book_Name = 2033;
        for (int i = 0; i < Writers_Value; i++)
        {
            Massive_Books_Sender[i] = Book_Name;
            Book_Name++;
        } 
        W_Time_Start = MPI_Wtime();
    }
    
    MPI_Scatter(Massive_Books_Sender, 1, MPI_INT, Massive_Books_Receiver, 1, MPI_INT, 0, MPI_COMM_WORLD);    //Send books to Writers from root-process (root process is Writer too)
    if (Current_Rank < Writers_Value)																		// if Writer - send to Reader
    {
        MPI_Send(&Massive_Books_Receiver[0], 1, MPI_INT, Current_Rank + Writers_Value, 500, MPI_COMM_WORLD);
        cout << "\n\nWriter '" << Current_Rank << "'\n\t Wrote a book METRO '" << Massive_Books_Receiver[0]<<"'";
    }
    else if (Current_Rank < Writers_Value*2)																// if Reader and have a oun Writer - receive the book from Writer
    {
        MPI_Recv(&Massive_Books_Receiver[0], 1, MPI_INT, Current_Rank - Writers_Value, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        cout << "\n\nReader '" << Current_Rank << "'\n\t Is reading a book METRO '" << Massive_Books_Receiver[0] << "'";
    }

    if (Current_Rank >= Writers_Value * 2)																	// if Reader dont have a book (Writer) - just view message
    {
        cout << "\n\nReader '" << Current_Rank << "' didn't get the book...";
    }
    if (Current_Rank == 0)
    {
        W_Time_End = MPI_Wtime();
        cout << "\n\n********************************************\nTotal time in parallel: " << W_Time_End - W_Time_Start << " seconds.\n********************************************";
    }
    MPI_Finalize();
}