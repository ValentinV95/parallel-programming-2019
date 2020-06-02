#include <vector>
#include <cstdlib>
#include <time.h>
#include <stdio.h>

void quickSort(std::vector <double>* vector, int left, int right) {
    while (right > left) 
	{
        int iterate_left = left;
        int iterate_right = right;
        double pivot = (*vector)[(left + right) >> 1];

        while (iterate_left <= iterate_right) {
            while ((*vector)[iterate_left] < pivot) 
			{
                iterate_left += 1;
            }
            while ((*vector)[iterate_right] > pivot) 
			{
                iterate_right -= 1;
            }
            if (iterate_left <= iterate_right) 
			{
                std::swap((*vector)[iterate_left], (*vector)[iterate_right]);
                iterate_left += 1;
                iterate_right -= 1;
            }
        }

        if ((iterate_left << 1) > left + right) 
		{
            quickSort(vector, iterate_left, right);
            right = iterate_left - 1;
        } 
		else 
		{
            quickSort(vector, left, iterate_left - 1);
            left = iterate_left;
        }
    }
}

void randomVec(int size, std::vector <double>* vector) 
{
	int i = 0;
	double number;
	while (i < size) 
	{
		number = rand() / (RAND_MAX + 1.0);
		(*vector).push_back(number);
		i += 1;
	}
}

void isSorted(std::vector <double>* vector) 
{
	for (int i = 0; i < (*vector).size() - 1; i++) 
	{
		if ( (*vector)[i] > (*vector)[i+1]) {printf("Incorrect sort %f %f", (*vector)[i], (*vector)[i + 1]); return;} 
	}
	printf("Correct sort");
}

int main() 
{
	srand(time(NULL));
	
	int n = 100000;
	std::vector <double> vec;
	
	randomVec(n, &vec);
    quickSort(&vec, 0, n - 1);
    isSorted(&vec);
    
    return 0;
}
