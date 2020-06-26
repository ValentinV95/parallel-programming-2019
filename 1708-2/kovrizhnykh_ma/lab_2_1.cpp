#include <cstdlib>
#include <time.h>
#include <stdio.h>
#include <limits.h>

void quickSort(double* array, int left, int right)
{
    while (right > left)
	{
        int iterate_left = left;
        int iterate_right = right;
        double pivot = array[(left + right) >> 1];

        while (iterate_left <= iterate_right)
		{
            while (array[iterate_left] < pivot)
			{
                iterate_left += 1;
            }
            while (array[iterate_right] > pivot)
			{
                iterate_right -= 1;
            }
            if (iterate_left <= iterate_right)
			{
                double temp = array[iterate_left];
                array[iterate_left] = array[iterate_right];
                array[iterate_right] = temp;

                iterate_left += 1;
                iterate_right -= 1;
            }
        }

        if ((iterate_left << 1) > left + right)
		{
            quickSort(array, iterate_left, right);
            right = iterate_left - 1;
        }
		else
		{
            quickSort(array, left, iterate_left - 1);
            left = iterate_left;
        }
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

	int size = 20000;
	int threads = 4;

    double* seq = new double[size];
    getRandomArray(seq, size);

    clock_t start = clock();
    quickSort(seq, 0, size - 1);
    clock_t end = clock();
	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
    printf("(Sequential) time for quicksort = %f \n", seconds);

    if (!isSorted(seq, size)) {
    	printf("Sorted incorrectly\n");
    } else {
		printf("Sorted correctly\n");
	}

    delete[]seq;

	return 0;
}
