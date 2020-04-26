#include "pch.h"
#include <omp.h>
#include <algorithm>
#include <ctime>
#include <random>
#include <vector>
#include <iostream>

std::vector<double> GetRandomVector(int size) {
	std::mt19937 gen;
	gen.seed(static_cast<unsigned int>(time(0)));
	std::vector<double> my_vector(size);
	for (int i = 0; i < size; ++i) {
		my_vector[i] = (gen() % 1000) / (i + 1.0);
		if (i % 2 == 0) my_vector[i] *= -1;
	}
	return my_vector;
}

int GettingMaxMantissa(std::vector<double> my_vector, int start, int end) {
	int max_power = 0;
	int power = 0;
	for (int i = start; i <= end; ++i) {
		power = 0;
		double mantis =
			std::abs(my_vector[i]) - static_cast<int>(std::abs(my_vector[i]));
		double dif = mantis;
		while (dif != 0 && power != 8) {
			power++;
			mantis *= 10;
			dif = mantis - static_cast<int>(mantis);
		}

		if (power > max_power) {
			max_power = power;
		}
	}
	return max_power;
}

int GettingMaxPower(std::vector<double>::iterator first,
	std::vector<double>::iterator last) {
	int max_power = 0;
	int power = 0;
	for (auto i = first; i != last; ++i) {
		power = 0;
		int tmp = static_cast<int>(*i);
		while (tmp != 0) {
			power++;
			tmp = tmp / 10;
		}
		if (power > max_power) {
			max_power = power;
		}
	}
	return max_power;
}

int GettingOnPower(double value, int power) {
	return std::abs(static_cast<int>(static_cast<int>(value) %
		(static_cast<int>(pow(10, power))) /
		(pow(10, power - 1))));
}

int GettingOnPowerMantissa(double value, int power, int max_power) {
	double mantis = (value - static_cast<int>(value)) * pow(10, max_power);

	return GettingOnPower(mantis, power);
}

void BitSortMantissa(std::vector<double>* my_vector, int start, int end,
	int power, int max_power) {
	std::vector<double> tmp_vector;

	for (int i = start; i <= end; ++i) {
		tmp_vector.push_back(my_vector->at(i));
	}

	std::vector<double> res_vector(tmp_vector);

	std::vector<int> counter(10, 0);
	int on_power = 0;

	for (unsigned int i = 0; i < tmp_vector.size(); ++i) {
		on_power = GettingOnPowerMantissa(tmp_vector[i], power, max_power);
		counter[on_power]++;
	}

	std::vector<int> counter_tmp(10, 0);

	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < i; ++j) {
			counter_tmp[i] += counter[j];
		}
	}

	for (unsigned int i = 0; i < tmp_vector.size(); ++i) {
		res_vector[counter_tmp[GettingOnPowerMantissa(tmp_vector[i], power,
			max_power)]++] = tmp_vector[i];
	}

	int id = 0;

	for (int i = start; i <= end; ++i) {
		my_vector->at(i) = res_vector[id++];
	}
}

void BitSort(std::vector<double>* my_vector, int power) {
	std::vector<double> res_vector(*my_vector);

	std::vector<int> counter(10, 0);
	int on_power = 0;

	for (unsigned int i = 0; i < my_vector->size(); ++i) {
		on_power = GettingOnPower(my_vector->at(i), power);
		counter[on_power]++;
	}

	std::vector<int> counter_tmp(10, 0);

	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < i; ++j) {
			counter_tmp[i] += counter[j];
		}
	}

	for (unsigned int i = 0; i < my_vector->size(); i++) {
		res_vector[counter_tmp[GettingOnPower(my_vector->at(i), power)]++] =
			my_vector->at(i);
	}

	for (unsigned int i = 0; i < my_vector->size(); i++) {
		my_vector->at(i) = res_vector[i];
	}
}

void DoppelgangerSort(std::vector<double>* my_vector) {
	for (unsigned int i = 0; i < my_vector->size() - 1; i++) {
		int doppelganger = 0;
		int start;
		int end;

		if (static_cast<int>(my_vector->at(i)) ==
			static_cast<int>(my_vector->at(i + 1))) {
			start = i;
			i++;
			doppelganger++;

			while (i < my_vector->size() - 1 &&
				(static_cast<int>(my_vector->at(i)) ==
					static_cast<int>(my_vector->at(i + 1)))) {
				i++;
				doppelganger++;
			}
			end = start + doppelganger;

			int power_mantis = GettingMaxMantissa(*my_vector, start, end);

			for (int j = 1; j <= power_mantis; j++) {
				BitSortMantissa(my_vector, start, end, j, power_mantis);
			}
		}
	}
}

void BitwiseSort(std::vector<double>::iterator first,
	std::vector<double>::iterator last) {
	std::vector<double> positiv_vector;
	std::vector<double> negativ_vector;

	int power = GettingMaxPower(first, last);

	for (auto i = first; i != last; ++i) {
		if (*i >= 0)
			positiv_vector.push_back(*i);
		else
			negativ_vector.push_back(*i);
	}

	if (negativ_vector.size() != 0) {
		for (int i = 1; i <= power; ++i) {
			BitSort(&negativ_vector, i);
		}
		DoppelgangerSort(&negativ_vector);
	}

	if (positiv_vector.size() != 0) {
		for (int i = 1; i <= power; ++i) {
			BitSort(&positiv_vector, i);
		}
		DoppelgangerSort(&positiv_vector);
	}

	auto id = first;

	for (int i = negativ_vector.size() - 1; i >= 0; --i) {
		*id = negativ_vector[i];
		id++;
	}

	for (unsigned int i = 0; i < positiv_vector.size(); i++) {
		*id = positiv_vector[i];
		id++;
	}
}

void MergeSort(std::vector<double>::iterator first,
	std::vector<double>::iterator middle,
	std::vector<double>::iterator end) {
	std::vector<double> sort_vector;
	auto i = first;
	auto j = middle;
	while (i != middle && j != end)
		if (*i < *j) {
			sort_vector.push_back(*i);
			i++;
		}
		else {
			sort_vector.push_back(*j);
			j++;
		}

	while (i != middle) {
		sort_vector.push_back(*i);
		i++;
	}

	while (j != end) {
		sort_vector.push_back(*j);
		j++;
	}

	int m = 0;

	for (auto k = first; k != end; k++) {
		*k = sort_vector[m++];
	}
}

void MPSort(std::vector<double>::iterator first,
	std::vector<double>::iterator end, int num_th) {
	if ((end - first) % num_th == 0) {
		int task_size = (end - first) / num_th;
		int task_size_old = task_size;
		omp_set_num_threads(num_th);

		int iam;
		int i = log(num_th) / log(2);
		int h = 2;
#pragma omp parallel private(iam)
		{
			iam = omp_get_thread_num();

			BitwiseSort(first + iam * task_size, first + (iam + 1) * task_size);
#pragma omp barrier

			while (i != 0) {
#pragma omp barrier
				if (iam == 0) {
					task_size_old = task_size;
					task_size += task_size;
				}
#pragma omp barrier

				if (iam < num_th / h) {
					MergeSort(first + iam * task_size,
						first + iam * task_size + task_size_old,
						first + iam * task_size + task_size);
				}

#pragma omp barrier
				if (iam == 0) {
					--i;
					h *= 2;
				}
#pragma omp barrier
			}
		}
	}
}

void OMPSort(std::vector<double>::iterator first,
	std::vector<double>::iterator end, int num_th) {
	if ((end - first) % num_th != 0) {
		std::vector<double>::iterator new_end = end;
		while ((new_end - first) % num_th != 0) {
			new_end--;
		}
		MPSort(first, new_end, num_th);
		BitwiseSort(new_end, end);
		MergeSort(first, new_end, end);
	}
	else {
		MPSort(first, end, num_th);
	}
}

bool IsSorted(std::vector<double>* old_vector,
	std::vector<double>* new_vector) {
	std::sort(old_vector->begin(), old_vector->end());

	bool flag = true;
	for (unsigned int i = 0; i < old_vector->size(); i++) {
		if (old_vector->at(i) != new_vector->at(i)) {
			flag = false;
			break;
		}
	}

	return flag;
}

int main(int argc, char** argv) {
	int size = atoi(argv[1]);
	std::vector<double> my_vector = GetRandomVector(size);
	std::vector<double> copy_vector(my_vector);
	OMPSort(my_vector.begin(), my_vector.end(), 4);
	bool flag = IsSorted(&copy_vector, &my_vector);
	if (flag == true)
		std::cout << "\n\tResult_Sorting: True";
	else
		std::cout << "\n\tResult_Sorting: False";
	return 0;
}