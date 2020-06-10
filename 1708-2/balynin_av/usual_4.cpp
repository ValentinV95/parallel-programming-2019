#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <cstring>
#include <climits>
#include <vector>

void CountingSort(double* inp, double* out, int byteNum, int size) {
	unsigned char* mas = (unsigned char*)inp;
	int counter[256];
	int tem;
	memset(counter, 0, sizeof(int) * 256);
	for (int i = 0; i < size; i++)
		counter[mas[8 * i + byteNum]]++;
	int j = 0;
	for (; j < 256; j++) {
		if (counter[j] != 0)
			break;
	}
	tem = counter[j];
	counter[j] = 0;
	j++;
	for (; j < 256; j++) {
		int b = counter[j];
		counter[j] = tem;
		tem += b;
	}
	for (int i = 0; i < size; i++) {
		out[counter[mas[8 * i + byteNum]]] = inp[i];
		counter[mas[8 * i + byteNum]]++;
	}
}

void LastCountingSort(double* inp, double* out, int byteNum, int size) {
	unsigned char* mas = (unsigned char*)inp;
	int counter[256];
	int tem;
	memset(counter, 0, sizeof(int) * 256);
	for (int i = 0; i < size; i++)
		counter[mas[8 * i + byteNum] + 128]++;
	int j = 0;
	for (; j < 256; j++) {
		if (counter[j] != 0)
			break;
	}
	tem = counter[j];
	counter[j] = 0;
	j++;
	for (; j < 256; j++) {
		int b = counter[j];
		counter[j] = tem;
		tem += b;
	}
	for (int i = 0; i < size; i++) {
		out[counter[mas[8 * i + byteNum] + 128]] = inp[i];
		counter[mas[8 * i + byteNum] + 128]++;
	}
}

void merge(double* mas, int sizel, int sizer) {
	int size = sizel + sizer;
	double* tempMas = new double[size];
	int i = 0, j = sizel, k = 0;

	while (i != sizel && j != size) {
		if (mas[i] <= mas[j]) {
			tempMas[k] = mas[i];
			++i;
			++k;
		}
		else {
			tempMas[k] = mas[j];
			++j;
			++k;
		}
	}

	if (i < sizel) {
		for (; i < sizel; ++i) {
			tempMas[k] = mas[i];
			++k;
		}
	}

	if (j < size) {
		for (; j < size; ++j) {
			tempMas[k] = mas[j];
			++k;
		}
	}

	for (i = 0; i < size; ++i) {
		mas[i] = tempMas[i];
	}

	delete[] tempMas;
}

std::vector<int> merge_size(std::vector<int> counts, int num_th) {
	std::vector<int> tmp;
	if (num_th % 2 == 1) {
		for (int i = 0; i < num_th / 2; ++i) {
			tmp.push_back(counts[2 * i] + counts[2 * i + 1]);
		}
		tmp.push_back(counts[counts.size() - 1]);
	}
	else {
		for (int i = 0; i < num_th / 2; ++i) {
			tmp.push_back(counts[2 * i] + counts[2 * i + 1]);
		}
	}
	return tmp;
}

int  displacement_M(std::vector<int> counts, int num_th) {
	int sum = 0;
	for (int i = 0; i < num_th; ++i) {
		sum += counts[2 * i] + counts[2 * i + 1];
	}
	return sum;
}

int  displacement_S(std::vector<int> counts, int num_th) {
	int sum = 0;
	for (int i = 0; i < num_th; ++i) {
		sum += counts[i];
	}
	return sum;
}

void PrintArray(double* array, int size) {
	if (size < 20) {
		for (int i = 0; i < size; i++) {
			std::cout << array[i] << " ";
		}
		std::cout << std::endl;
	}
}

void LSDSortDouble(double* inp, int size) {
	double* out = new double[size];

	for (int i = 0; i < 6; i += 2) {
		CountingSort(inp, out, i, size);
		CountingSort(out, inp, i + 1, size);
	}
	CountingSort(inp, out, 6, size);
	LastCountingSort(out, inp, 7, size);
	delete[] out;
}

void CopyArray(double* mas, double* tmp, int size) {
	for (int i = 0; i < size; i++)
		tmp[i] = mas[i];
}

void CheckingSort(double* mas, double* tmp, int size) {
	for (int i = 0; i < size; i++) {
		if (mas[i] != tmp[i]) {
			std::cout << "Sort is incorrectly" << std::endl;
			break;
		}
	}
	std::cout << "Sort is correctly" << std::endl;
}

void GenerateArray(double* mas, int size) {
	int b = 100;
	int a = 0;
	for (int i = 0; i < size; i++) {
		mas[i] = static_cast<double>(std::rand()) * (b - a + 1) / RAND_MAX + a;
	}
}

int main(int argc, char* argv[]) {
	std::vector<int> counts;
	int size = 10;
	std::srand((unsigned)time(NULL));
	double* mas, *tmp, *lmas;
	size = atoi(argv[1]);
	mas = new double[size];
	tmp = new double[size];
	lmas = new double[size];
	counts.push_back(size);
	GenerateArray(mas, size);
	CopyArray(mas, tmp, size);
	std::sort(tmp, tmp + size);
	CopyArray(mas, lmas, size);
	if (mas == NULL) {
		std::cout << "Error! Incorrect input data for array";
		return -1;
	}
	std::cout << "Array after LSD sort: ";
	PrintArray(lmas, size);
	std::cout << "LSD ";
	CheckingSort(lmas, tmp, size);
	std::cout << "Array after LSD sort with simle merge: ";
	PrintArray(mas, size);
	std::cout << "LSD with simple merge ";
	CheckingSort(mas, tmp, size);
	std::cout << "Execution time LSD sort: " << std::endl;
	delete[] mas;
	delete[] tmp;
	delete[] lmas;
	return 0;
}