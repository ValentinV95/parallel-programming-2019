#include "pch.cpp"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <list>
#include <Windows.h>
#include "mpi.h"


void philosopher(int);
void table(int, int);
#define NEED_FORK 1
#define FORK_RESPONSE 2
#define FORK_RELEASE 3


/* Philosopher function - only philosopher processes run this */
void philosopher(int myrank) {
  printf("Hello from philosopher %d \n", myrank);
  int in_buffer[1];
  int out_buffer[1];
  MPI_Status stat;
  out_buffer[0];
  srand(time(NULL) + myrank);

  //Philosopher main loop
  while (true) {
    printf("Philosopher %d is sleeping \n", myrank);
    Sleep(rand() % 10); //Sleep
    printf("Philosopher %d is waiting to eat \n", myrank);
    MPI_Send(out_buffer, 1, MPI_INT, 0, NEED_FORK, MPI_COMM_WORLD); //Request forks
    MPI_Recv(in_buffer, 1, MPI_INT, 0, FORK_RESPONSE, MPI_COMM_WORLD, &stat); //Wait for response
    printf("Philosopher %d is eating \n", myrank);
    Sleep(rand() % 10); //Eat
    printf("Philosopher %d is done eating \n", myrank);
    MPI_Send(out_buffer, 1, MPI_INT, 0, FORK_RELEASE, MPI_COMM_WORLD); //Release forks
  }
}

/* Table function - only table process run this */
void table(int myrank, int nprocs, int sizew) {
  printf("Hello from table %d \n", myrank);
  int in_buffer[1];
  int out_buffer[1];
  int philosopher;
  MPI_Status stat;

  std::list<int> queue;

  bool *fork = new bool[sizew];
  for (int i = 0; i < sizew; i++) fork[i] = true; //Init all forks as free
  //Table main loop
  while (true) {
    MPI_Recv(in_buffer, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat); // Recive next message
    philosopher = stat.MPI_SOURCE; //Read source of message

    if (stat.MPI_TAG == NEED_FORK) { //If Request for forks
      printf("Table got philosopher %d fork request\n", philosopher);
      if (fork[philosopher % (sizew)] == true && fork[philosopher - 1] == true) { //If both forks are free
        fork[philosopher % (sizew)] = false; //Set the forks as taken
        fork[philosopher - 1] = false;
        MPI_Send(out_buffer, 1, MPI_REAL, philosopher, FORK_RESPONSE, MPI_COMM_WORLD); // Send Fork response to the right philosopher
        printf("Table sent philosopher %d the forks\n", philosopher);
      }
      else //If not both forks are free
        queue.push_back(philosopher); //Put in wait queue
    }
    if (stat.MPI_TAG == FORK_RELEASE) { //If Release of forks
      fork[philosopher % (sizew)] = true; //Set forks to free again
      fork[philosopher - 1] = true;
      printf("Table got philosopher %d fork release\n", philosopher);

      if (!queue.empty()) { //If philosopher waiting for forks
        for (std::list<int>::iterator it = queue.begin(); it != queue.end(); it++) { //Go through whole list of waiting philosophers
          philosopher = *it;
          if (fork[philosopher % (sizew)] == true && fork[philosopher - 1] == true) { //If one of them can get both forks
            fork[philosopher % (sizew)] = false;
            fork[philosopher - 1] = false;
            MPI_Send(out_buffer, 1, MPI_INT, philosopher, FORK_RESPONSE, MPI_COMM_WORLD); // send Fork response
            printf("Table sent philosopher %d the forks\n", philosopher);
            it = queue.erase(it); //Remove from wait list
          }
        }
      }
    }
  }
}



int main(int argc, char** argv) {
  int myrank, nprocs;
  //init MPI
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  int sizew = atoi(1);

  //Depending on rank, Philosopher or Table
  if (myrank == 0)
  {
    table(myrank, nprocs, sizew);
  }
  else
  {
    philosopher(myrank);
  }

  MPI_Finalize();
  return 0;
}