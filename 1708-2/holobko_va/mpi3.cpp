#include "pch.h"
#include <time.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <mpi.h>
#include <vector>
#include <time.h>
#include <stdexcept>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <random>
#include <ctime>

using namespace std;

std::vector<int> BSort(std::vector<int> vec)
{
	int temp;
	for (int i = 0; i < vec.size() - 1; i++) {
		for (int j = 0; j < vec.size() - i - 1; j++) {
			if (&vec[j] > &vec[j + 1]) {
				temp = vec[j];
				vec[j] = vec[j + 1];
				vec[j + 1] = temp;
			}
		}
	}
	return vec;
}

std::vector<int> GetRandVec(int size) {
	if (size <= 0)
		throw - 1;
	std::vector<int> vec(size);
	std::mt19937 el;
	el.seed(static_cast<unsigned int>(time(0)));
	for (int i = 0; i < size; i++) {
		vec[i] = el() % 1000000;
	}
	return vec;
}

std::vector<int> Radix_sort_iter(const std::vector<int>& source, int n, int iter) {
	int* count = new int[256];
	std::vector<int> res(source.size());
	unsigned char* br = (unsigned char*)source.data() + iter;
	for (int i = 0; i < 256; i++)
		count[i] = 0;

	unsigned char tmp;
	for (int i = 0; i < n; i++) {
		tmp = br[i * 4];
		count[tmp]++;
	}

	int sum = 0;
	if (iter == 3) {
		for (int i = 128; i < 256; i++) {
			sum += count[i];
			count[i] = sum - count[i];
		}

		for (int i = 0; i < 128; i++) {
			sum += count[i];
			count[i] = sum - count[i];
		}
	}
	else {
		for (int i = 0; i < 256; i++) {
			sum += count[i];
			count[i] = sum - count[i];
		}
	}

	for (int i = 0; i < n; i++) {
		res[count[*br]] = source[i];
		count[*br]++;
		br += 4;
	}
	delete[] count;
	return res;
}

std::vector<int> Radix_sort(const std::vector<int>& vec) {
	int length = vec.size();
	std::vector<int> res(vec);

	for (int i = 0; i < 4; i++)
		res = Radix_sort_iter(res, length, i);

	return res;
}

std::vector<int> Merge_sort(const std::vector<int>& loc_vec, int loc_size,
	const std::vector<int>& neig_vec, int neig_size) {
	std::vector<int> res(loc_size + neig_size);
	int l = 0, n = 0, r = 0;
	while (l < loc_size && n < neig_size) {
		if (loc_vec[l] < neig_vec[n]) {
			res[r] = loc_vec[l];
			r++;
			l++;
		}
		else {
			res[r] = neig_vec[n];
			n++;
			r++;
		}
	}
	while (l < loc_size) {
		res[r] = loc_vec[l];
		r++;
		l++;
	}
	while (n < neig_size) {
		res[r] = neig_vec[n];
		r++;
		n++;
	}
	return res;
}

int pow2(int st) {
	int res = 1;
	for (int i = 0; i < st; i++)
		res *= 2;
	return res;
}

std::vector<int> Par_Radix_sort(const std::vector<int>& source) {
	int rank, size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int length, ost;
	if (rank == 0) {
		length = source.size() / size;
		ost = source.size() % size;
	}
	MPI_Bcast(&length, 1, MPI_INT, 0, MPI_COMM_WORLD);

	std::vector<int> local_vec(length);
	MPI_Scatter(&source[0], length, MPI_INT, &local_vec[0], length, MPI_INT, 0, MPI_COMM_WORLD);
	if (rank == 0 && ost != 0) {
		local_vec.insert(local_vec.end(), source.end() - ost, source.end());
	}

	local_vec = Radix_sort(local_vec);

	int i = size;
	int iter = 1;
	int iter_length = length;
	int sosed;
	int loc_size = length;
	if (rank == 0)
		loc_size += ost;
	while (i > 1) {
		if (i % 2 == 1) {
			if (rank == 0) {
				MPI_Status status1;
				std::vector<int>neig_vec(iter_length);
				sosed = pow2(iter - 1) * (i - 1);
				MPI_Recv(&neig_vec[0], iter_length, MPI_INT, sosed, 1, MPI_COMM_WORLD, &status1);
				local_vec = Merge_sort(local_vec, loc_size, neig_vec, iter_length);
				loc_size += iter_length;
			}
			if (rank == pow2(iter - 1) * (i - 1)) {
				MPI_Send(&local_vec[0], iter_length, MPI_INT, 0, 1, MPI_COMM_WORLD);
				return local_vec;
			}
		}

		if (rank % pow2(iter) == 0) {
			sosed = rank + pow2(iter - 1);
			std::vector<int> neig_vec(iter_length);
			MPI_Status status3;
			MPI_Recv(&neig_vec[0], iter_length, MPI_INT, sosed, 3, MPI_COMM_WORLD, &status3);
			local_vec = Merge_sort(local_vec, loc_size, neig_vec, iter_length);
			loc_size += iter_length;
		}
		if (rank % pow2(iter) != 0) {
			sosed = rank - pow2(iter - 1);
			MPI_Send(&local_vec[0], iter_length, MPI_INT, sosed, 3, MPI_COMM_WORLD);
			return local_vec;
		}
		iter++;
		i = i / 2;
		iter_length *= 2;
	}
	return local_vec;
}

int main(int argc, char** argv) {
	double st1, st2, et1, et2;
	MPI_Init(&argc, &argv);
	int rank, size, SizeV = 10,lc=0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	SizeV = atoi(argv[1]);
	std::vector<int> bvec;
	std::vector<int> vec;
	std::vector<int> res;
	vec = GetRandVec(SizeV);
	if (rank == 0) {
		st1 = MPI_Wtime();
		bvec = BSort(vec);
	}
	st2 = MPI_Wtime();
	res = Par_Radix_sort(vec);

	if (rank == 0) {
		et1 = MPI_Wtime()-st1;
		et2 = MPI_Wtime()-st2;
		cout << "Time_Paralel_Bitwise_Sort:" << et2 << std::fixed << " sec \n";
		cout << "Time_Sinlgle_Bubbly_Sort:" << et1  << std::fixed << " sec \n";
		if (et1 < et2)
			cout << "Sinlgle_Bubbly_Sort:_Faster \n";
		else
			cout << "Paralel_Bitwise_Sort:_Faster \n";
		for (int i = 0; i <= SizeV;i++) {
			if (vec[i] != res[i])
				lc++;
		}
		if (lc != 0)
			cout << "Sort_Will_Compleate \n";
		else
			throw - 1;
	}
	MPI_Finalize();
	return 0;
}