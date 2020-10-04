#include <omp.h>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <vector>

using namespace std;

void ArrGen(int* arr, int size, int min, int max) {
	for (int i = 0; i < size; i++)
		arr[i] = static_cast<int>(rand()) * (max - min + 1) / RAND_MAX + min;
}

void ArrCopy(int* arr, int* tmp, int size) {
	for (int i = 0; i < size; i++)
		tmp[i] = arr[i];
}

void CountingSort(int* inp, int* outp, int bNum, int size) {
	unsigned char* arr = (unsigned char*)inp;
	int counter[256];
	int temp;
	memset(counter, 0, sizeof(int) * 256);
	for (int i = 0; i < size; i++)
		counter[arr[4 * i + bNum]]++;
	int j = 0;
	for (; j < 256; j++) 
		if (counter[j] != 0)
			break;
	temp = counter[j];
	counter[j] = 0;
	j++;
	for (; j < 256; j++) 
	{
		int b = counter[j];
		counter[j] = temp;
		temp += b;
	}
	for (int i = 0; i < size; i++) 
	{
		outp[counter[arr[4 * i + bNum]]] = inp[i];
		counter[arr[4 * i + bNum]]++;
	}
}

void LastCountingSort(int* inp, int* outp, int bNum, int size) {
	unsigned char* arr = (unsigned char*)inp;
	int counter[256];
	int temp;

	memset(counter, 0, sizeof(int) * 256);
	for (int i = 0; i < size; i++)
		counter[arr[4 * i + 3] + 128]++;
	int j = 0;
	for (; j < 256; j++) 
		if (counter[j] != 0)
			break;
	temp = counter[j];
	counter[j] = 0;
	j++;
	for (; j < 256; j++) 
	{
		int b = counter[j];
		counter[j] = temp;
		temp += b;
	}
	for (int i = 0; i < size; i++) 
	{
		outp[counter[arr[4 * i + bNum] + 128]] = inp[i];
		counter[arr[4 * i + bNum] + 128]++;
	}
}

void LSDSort(int* inp, int size) {
	int* outp = new int[size];

	CountingSort(inp, outp, 0, size);
	CountingSort(outp, inp, 1, size);
	CountingSort(inp, outp, 2, size);
	LastCountingSort(outp, inp, 3, size);
	delete[] outp;
}

void merge(int* arr, int sizel, int sizer) {

	int size = sizel + sizer;
	int* tArr = new int[size];
	int i(0), j(sizel), k(0);

	while (i != sizel && j != size) 
	{
		if (arr[i] <= arr[j]) 
		{
			tArr[k] = arr[i];
			++i;
			++k;
		}
		else 
		{
			tArr[k] = arr[j];
			++j;
			++k;
		}
	}

	if (i < sizel) 
		for (; i < sizel; ++i) 
		{
			tArr[k] = arr[i];
			++k;
		}

	if (j < size) 
		for (; j < size; ++j) 
		{
			tArr[k] = arr[j];
			++k;
		}

	for (i = 0; i < size; ++i) 
		arr[i] = tArr[i];

	delete[] tArr;
}

vector<int> merge_size(vector<int> counts, int nt) {
	vector<int> tmp;
	if (nt % 2 == 1) 
	{
		for (int i = 0; i < nt / 2; ++i) 
			tmp.push_back(counts[2 * i] + counts[2 * i + 1]);
		tmp.push_back(counts[counts.size() - 1]);
	}
	else 
	{
		for (int i = 0; i < nt / 2; ++i) 
			tmp.push_back(counts[2 * i] + counts[2 * i + 1]);
	}
	return tmp;
}

int  offset_M(vector<int> counts, int nt) {
	int sum(0);

	for (int i = 0; i < nt; ++i) 
		sum += counts[2 * i] + counts[2 * i + 1];
	return sum;
}

int  offset_S(vector<int> counts, int nt) {
	int sum(0);

	for (int i = 0; i < nt; ++i) 
		sum += counts[i];
	return sum;
}

void ArrShow(int* array, int size) {
	for (int i = 0; i < size; i++) 
		cout << array[i] << " ";
	cout << endl;
}

void ArrShowAll(int* array1, int* array2, int* array3, int size) {
	ArrShow(array1, size);
	ArrShow(array2, size);
	ArrShow(array3, size);
	cout << endl;
}

void InfoShow(int n, int size, double TLSD, double TLSDp)
{
	cout << "######################## INFO ########################" << endl;
	cout << "# Num threads:                              " << n << endl;
	cout << "# Size:                                     " << size << endl;
	cout << "# LSD sort time:                            " << TLSD << endl;
	cout << "# Parallel LSD sort with simple merge time: " << TLSDp << endl;
	cout << "# Boost (TLSD/TLSDp):                       " << TLSD / TLSDp << endl;
	cout << "######################################################\n" << endl;
}

void CheckingSort(int* arr, int* tmp, int size) {
	for (int i = 0; i < size; i++)
	{
		if (arr[i] != tmp[i])
		{
			cout << "ERROR" << endl;
			break;
		}
	}
	cout << "PASSED" << endl;
}

void LogShow(int *arr, int *s_arr, int *tmp, int size)
{
	cout << "#################### LOG #####################" << endl;
	cout << "# LSD sort:                            ";
	CheckingSort(s_arr, tmp, size);
	cout << "# Parallel LSD sort with simple merge: ";
	CheckingSort(arr, tmp, size);
	cout << "##############################################\n" << endl;
}

int main(int argc, char* argv[]) {

	double TLSD(0), TLSDp(0);
	int size(1000000), n(4), tail(0);
	int *arr, *tmp, *s_arr;
	vector<int> counts;

	srand(time(NULL));

	if (argc == 4) {
		n = atoi(argv[1]);
		if (strcmp(argv[2], "-size") == 0)
			size = atoi(argv[3]);
	}

	if (size <=0 || n <= 0)
	{
		cout << "Incorrect size or n!\n";
		return -1;
	}


	arr = new int[size];
	s_arr = new int[size];
	tmp = new int[size];
	tail = size % n;

	for (int i = 0; i < n; ++i) 
	{
		if (i == 0) 
			counts.push_back(size / n + tail);
		else 
			counts.push_back(size / n);
	}

	ArrGen(arr, size, 0, 100);
	ArrCopy(arr, tmp, size);
	sort(tmp, tmp + size);
	ArrCopy(arr, s_arr, size);

	omp_set_num_threads(n);
	TLSDp = omp_get_wtime();
#pragma omp parallel
	{
		LSDSort(arr + offset_S(counts, omp_get_thread_num()), counts[omp_get_thread_num()]);
#pragma omp barrier
	}
	int f = n;
	int k = n / 2 + n % 2;

	while (k > 0) 
	{
		omp_set_num_threads(k);
#pragma omp parallel
		{
			if (f % 2 == 1)
			{
				if (omp_get_thread_num() != k - 1)
				{
					merge(arr + offset_M(counts, omp_get_thread_num()), counts[2 * omp_get_thread_num()], counts[2 * omp_get_thread_num() + 1]);
				}
			}
			else
			{
				merge(arr + offset_M(counts, omp_get_thread_num()), counts[2 * omp_get_thread_num()], counts[2 * omp_get_thread_num() + 1]);
			}
#pragma omp barrier
			if (omp_get_thread_num() == 0) 
			{
				counts = merge_size(counts, f);
				if (k == 1) 
					k = 0;
				else 
					k = k / 2 + k % 2;
				f = f / 2 + f % 2;
			}
		}
	}

	TLSDp = omp_get_wtime() - TLSDp;

	TLSD = omp_get_wtime();
	LSDSort(s_arr, size);
	TLSD = omp_get_wtime() - TLSD;


	InfoShow(n, size, TLSD, TLSDp);
	LogShow(arr, s_arr, tmp, size);

	delete[] arr;
	delete[] tmp;
	delete[] s_arr;
	system("pause");
	return 0;
}