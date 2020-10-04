#include <tbb/tbb.h>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <vector>

using namespace std;
using namespace tbb;

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

void InfoShow(int n, int size, double TLSD, double TLSDp, double mT, double pTLSD)
{
	cout << "######################## INFO ########################" << endl;
	cout << "# Num threads:                              " << n << endl;
	cout << "# Size:                                     " << size << endl;
	cout << "# LSD sort time:                            " << TLSD << endl;
	//cout << "# LSD sort with simple merge time:          " << TLSDp << endl;
	//cout << "# Merge time:                               " << mT << endl;
	cout << "# Parallel LSD sort time:                   " << pTLSD << endl;
	cout << "# Boost (TLSD/pTLSD):                       " << TLSD / pTLSD << endl;
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

	double TLSD(0), TLSDp(0), mT(0), pTLSD(0);
	int size(10000000), n(4), tail(0);
	int *arr, *tmp, *s_arr;
	vector<int> counts;

	srand(time(NULL));

	if (argc == 4) {
		n = atoi(argv[1]);
		if (strcmp(argv[2], "-size") == 0)
			size = atoi(argv[3]);
	}

	if (size <= 0 || n <= 0)
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

	task_scheduler_init init(n);
	tick_count pTLSD_ts = tick_count::now();

	tick_count TLSDp_ts = tick_count::now();

	parallel_for(blocked_range<int>(0, n), [=, &arr](const blocked_range<int> &r) {
		for (int f = r.begin(); f != r.end(); ++f) {
			LSDSort(arr + offset_S(counts, f), counts[f]);
		}
	});
	tick_count TLSDp_tf = tick_count::now();

	init.terminate();
	int j = n;
	int k = n / 2 + n % 2;
	tick_count mT_ts = tick_count::now();
	while (k > 0) {
		init.initialize(k);
		parallel_for(blocked_range<int>(0, k), [=, &arr](const blocked_range<int> &r) {
			for (int f = r.begin(); f != r.end(); ++f) {
				if (j % 2 == 1) {
					if (f != k - 1) {
						merge(arr + offset_M(counts, f), counts[2 * f], counts[2 * f + 1]);
					}
				}
				else {
					merge(arr + offset_M(counts, f), counts[2 * f], counts[2 * f + 1]);
				}
			}
		});
		counts = merge_size(counts, j);
		if (k == 1) {
			k = 0;
		}
		else {
			k = k / 2 + k % 2;
		}
		j = j / 2 + j % 2;
		init.terminate();
	}

	tick_count mT_tf = tick_count::now();

	tick_count pTLSD_tf = tick_count::now();
	pTLSD = (pTLSD_tf - pTLSD_ts).seconds();

	tick_count TLSD_ts = tick_count::now();
	LSDSort(s_arr, size);
	tick_count TLSD_tf = tick_count::now();
	TLSD = (TLSD_tf - TLSD_ts).seconds();

	TLSDp = (TLSDp_tf - TLSDp_ts).seconds();

	mT = (mT_tf - mT_ts).seconds();


	InfoShow(n, size, TLSD, TLSDp, mT, pTLSD);
	LogShow(arr, s_arr, tmp, size);

	delete[] arr;
	delete[] tmp;
	delete[] s_arr;
	system("pause");
	return 0;
}