#include <cstdlib>
#include <time.h>
#include <stdio.h>
#include <limits.h>
#include <iostream>

void qsrt(double* arr, int low, int high) {
	int i;
	double pivot, t;
    if (low < high) {
        i = (low - 1);
    	pivot = arr[high];

    	for (int j = low; j <= high - 1; j++)
    	{
        	if (arr[j] <= pivot)
        	{
            	i++;
            	t = arr[i];
    			arr[i] = arr[j];
    			arr[j] = t;
        	}
    	}
    	t = arr[i + 1];
    	arr[i + 1] = arr[high];
    	arr[high] = t;
        qsrt(arr, low, i);

		i = (low - 1);
    	pivot = arr[high];

    	for (int j = low; j <= high - 1; j++)
    	{
        	if (arr[j] <= pivot)
        	{
            	i++;
            	t = arr[i];
    			arr[i] = arr[j];
    			arr[j] = t;
        	}
    	}
    	t = arr[i + 1];
    	arr[i + 1] = arr[high];
    	arr[high] = t;
        qsrt(arr, i + 1, high);
    }
}

void getRandomArray(double* arr, int size)
{
	int i = 0;
	double number;

	while(i < size)
	{
		number = rand() / (RAND_MAX + 1.0);
		arr[i] = number;
		i += 1;
	}
}

bool isSorted(double* ar, int size) {
    const double *previous_value = ar;

    while (size) {
       if (*ar < *previous_value)
             return false;
       previous_value = ar;

       ++ar;
       --size;
     }
     return true;
}

int main(void) {
	srand(time(NULL));

	int size = 200000;
	int threads = 4;

    double* seq = new double[size];
    getRandomArray(seq, size);

    clock_t start = clock();
    qsrt(seq, 0, size - 1);
    clock_t end = clock();
	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
    printf("(Sequential) time for quicksort = %f \n", seconds);

    if (!isSorted(seq, size)) {
    	printf("Sorted incorrectly\n");
    } else {
		printf("Sorted correctly\n");
	}

	//print everything
	/*
	for (int i = 0; i < size; i++)
        std::cout << seq[i] << " ";
    std::cout << std::endl;*/

    delete[]seq;

	return 0;
}

