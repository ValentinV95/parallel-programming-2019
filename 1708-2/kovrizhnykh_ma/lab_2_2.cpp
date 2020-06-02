#include <omp.h>
#include <vector>
#include <cstdlib>
#include <climits>
#include <time.h>
#include <stdio.h>

void quickSort(std::vector <double>* vector, int left, int right) 
{
    while (right > left) 
	{
        int iterate_left = left;
        int iterate_right = right;
        double pivot = (*vector)[(left + right) >> 1];

        while (iterate_left <= iterate_right) 
		{
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

void even(int start, long int left_part, long int right_part, std::vector <double>* vector) 
{
    std::vector <double> temporary(left_part);
    
    long int i = 0;
    
    while (i < left_part) 
	{
    	temporary[i] = (*vector)[start + i];
    	i += (1 << 1);
    }

    right_part = start + left_part + right_part;
    
    long int first_iter = 0;
    long int second_iter = start + left_part;
    long int common_iter = start;
    
    for (; first_iter < left_part && second_iter < right_part; common_iter += 2) 
	{
    	if (temporary[first_iter] < (*vector)[second_iter]) 
		{
            (*vector)[common_iter] = temporary[first_iter];
            first_iter += 2;
        } 
		else 
		{
            (*vector)[common_iter] = (*vector)[second_iter];
            second_iter += 2;
        }
    }
    
    if (first_iter >= left_part) 
	{
    
    	i = second_iter;
    	
    	while (i < right_part) 
		{
    		(*vector)[common_iter] = (*vector)[i];
    		i += 2;
    		common_iter += 2;
    	}
    } 
	else 
	{   
        i = first_iter;
        while (i < left_part) 
		{
    		(*vector)[common_iter] = temporary[i];
    		i += 2;
    		common_iter += 2;
    	}
    }
}

void odd(int start, long int left_part, long int right_part, std::vector <double>* vector) 
{
    std::vector <double> temporary(left_part);
    
    long int i = 1;
    
    while (i < left_part) 
	{
    	temporary[i] = (*vector)[start + i];
    	i += (1 << 1);
    }
    
    right_part = start + left_part + right_part;
    
    long int first_iter = 1;
    long int second_iter = start + left_part + 1;
    long int common_iter = start + 1;
    
    for (; first_iter < left_part && second_iter < right_part; common_iter += 2) 
	{
    	if (temporary[first_iter] < (*vector)[second_iter]) 
		{
            (*vector)[common_iter] = temporary[first_iter];
            first_iter += 2;
        } 
		else 
		{
            (*vector)[common_iter] = (*vector)[second_iter];
            second_iter += 2;
        }
    }
    
    if (first_iter >= left_part) 
	{
    
    	i = second_iter;
    	
    	while (i < right_part) 
		{
    		(*vector)[common_iter] = (*vector)[i];
    		i += 2;
    		common_iter += 2;
    	}
    } 
	else 
	{   
        i = first_iter;
        while (i < left_part) 
		{
    		(*vector)[common_iter] = temporary[i];
    		i += 2;
    		common_iter += 2;
    	}
    }
}

void quickSortParallel(int threads, int size, std::vector <double>* vector) 
{
    omp_set_num_threads(threads);
    
    int delta;
    
    int deltaA = size / threads + (size / threads) % 2;
    int deltaB = (size - (threads - 1) * deltaA) + (size - (threads - 1) * deltaA) % 2;

    delta = (threads - 1) * deltaA;
    delta += deltaB;
    delta -= size;
    
    int i = 0;
    while (i < delta) 
	{
    	vector->push_back(INT_MAX);
    	i += 1;
    }
    
    int number_of_threads = threads;
#pragma omp parallel
    {
        #pragma omp for
        
        for (i = 0; i < number_of_threads; i += 1) 
		{
        	int left = deltaA * i;
			int right = size + delta - 1;
			
            if ( i + 1 != number_of_threads ) 
			{
            	right = left + deltaA - 1;
            }
            
            quickSort(vector, left, right);
        }
    }
    int count_m = number_of_threads >> 1;
    int k = 1;
    int last = 0;
    
    while (count_m > 0)
	{
#pragma omp parallel
        {
            if (omp_get_thread_num() < (count_m << 1)) 
			{
            	int first = deltaA * k;
            	int second = deltaA * last + deltaB;

                if (!(omp_get_thread_num() % 2)) 
				{
                	if (omp_get_thread_num() == number_of_threads - 2) 
					{
                		even(omp_get_thread_num() * first, first, second, vector);
                	} 
					else 
					{
                		even(omp_get_thread_num() * first, first, first, vector);
                	}
                }
				else 
				{
                	if (omp_get_thread_num() == number_of_threads - 1) 
					{
                		odd((omp_get_thread_num() - 1) * first, first, second, vector);
                	} 
					else 
					{
                		odd((omp_get_thread_num() - 1) * first, first, first, vector);
                	}
                }
            }
        }
#pragma omp parallel
        {
            int thread = omp_get_thread_num();
            if (thread < (count_m << 1)) 
			{
                if (!(thread % 2)) 
				{
                    int start = omp_get_thread_num() * deltaA * k + 1;
                                          
                    int final = (*vector).size() - 1;
                    
                    if ( (omp_get_thread_num() + 1) != (number_of_threads - 1) ) 
					{
                    	final = (omp_get_thread_num() + 2) * deltaA * k;
                    }

                    for (int i = start; i < final; i += (2 << 1)) 
					{
                        if ((*vector)[i] > (*vector)[i + 1]) 
						{
                            std::swap((*vector)[i], (*vector)[i + 1]);
                        }
                    }
                    
                } 
				else
				{
                    int start = (omp_get_thread_num() - 1) * deltaA * k + 3;
                    int final = (*vector).size() - 1;
                    
                    if (omp_get_thread_num() != (number_of_threads - 1)) 
					{
                    	final = (omp_get_thread_num() + 1) * deltaA * k;
                    }

                    for (int i = start; i < final - 1; i += (2 << 1)) 
					{
                        if ((*vector)[i] > (*vector)[i + 1]) 
						{
                            std::swap((*vector)[i], (*vector)[i + 1]);
                        }
                    }
                }
            }
        }
        if (!(number_of_threads % 2)) 
		{
            last += k;
        }
        number_of_threads -= count_m;
        count_m = number_of_threads >> 1;
        k <<= 1;
    }
    
    i = 0;
    while (i < delta) 
	{
    	vector->pop_back();
    	i += 1;
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
		if ( (*vector)[i] > (*vector)[i+1])		{printf("Incorrect sort %f %f", (*vector)[i], (*vector)[i + 1]); return;} 
	}
	printf("Correct sort");
}

int main(void) 
{
    srand(time(NULL));
	
	int n = 999999;
	std::vector <double> vec, vec2;
	
	randomVec(n, &vec);
	vec2 = vec;
	
	clock_t start = clock();
	
    quickSortParallel(8, n, &vec);
    
    clock_t end = clock();
	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
    printf("(OMP) quicksort = %f \n", seconds);
    
    
    start = clock();
    quickSort(&vec2, 0, n - 1);
    end = clock();
    seconds = (float)(end - start) / CLOCKS_PER_SEC;
    printf("(Sequential) quicksort = %f \n", seconds);
    
    isSorted(&vec);
    isSorted(&vec2);
    
    if ( vec == vec2 ) 
	{
    	puts("\nequal");
    }
    return 0;
}
