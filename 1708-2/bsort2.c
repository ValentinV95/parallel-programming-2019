#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100000
//swaping in the first method
void swap(int *r, int *o) {
  int temp = *r;
  *r = *o;
  *o = temp;
}

// A function to implement bubble sort
void bubbleSort(int arr[], int n) {
  int i, j;
  for (i = 0; i < n-1; i++)      
    for (j = 0; j < n-i-1; j++)
      if (arr[j] > arr[j+1])
	swap(&arr[j], &arr[j+1]);
}

int isSorted(int *a, int size) {
  int i;
  for (i = 0; i < size - 1; i++) {
    if (a[i] > a[i + 1]) {
      return 0;
    }
  }
  return 1;
}

// Function to print an array
void printArray(int arr[], int size)
{
  int i;
  for (i=0; i < size; i++)
    printf("%d ", arr[i]);
  printf("\n");
}


int main(int argc, char** argv) {
	int i, n;
  int* A;
	int* buffer;
	clock_t start, end;
	double elapsed_time, t1, t2;

	MPI_Init(NULL, NULL);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);


	t1 = MPI_Wtime();
  A = (int *)malloc(sizeof(int)*N);
	buffer = (int *)malloc(sizeof(int)*N);
  if (A == NULL || buffer == NULL) {
    printf("Fail to malloc\n");
    exit(0);
  }
  for (i=N-1; i>=0; i--)
    A[N-1-i] = i;
	
	// if (isSorted(A, N))
	//   printf("Array is sorted\n");
	// else
	//   printf("Array is NOT sorted\n");
	
  // TODO: Check if modulo is not zero
  int splitted_size = (N / world_size);
  int from = ((splitted_size * world_rank) + 1) - 1;
  int to = (splitted_size * (world_rank + 1)) - 1;
  // printf("%d - %d\n", from, to);

  bubbleSort(&A[from], splitted_size);

  if (world_rank != 0) {
    MPI_Send(&A[from], splitted_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
  } else {
    
    for (int i = 1; i < world_size; i++) {
      MPI_Recv(buffer, splitted_size, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      int start_at = ((splitted_size * i) + 1) - 1;
      for (int k = start_at, l = 0; k < start_at + splitted_size; k++, l++) {
        A[k] = buffer[l];
      }
    }

    // Merge result !
    // bubbleSort(A, N);

    printArray(&A[0], N);
     
    t2 = MPI_Wtime();
    printf( "Elapsed time MPI_Wtime is %f\n", t2 - t1 ); 
  } 

	MPI_Finalize();
	return 0;
}
