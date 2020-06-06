#include <iostream>
#include <stdio.h>
#include <time.h>
#include <ctime>
using namespace std;


void Merge(int* a, int* b, int l, int m, int r)
{
	int h, i, j, k;
	h = l;
	i = l;
	j = m + 1;

	while ((h <= m) && (j <= r))
	{
		if (a[h] <= a[j])
		{
			b[i] = a[h];
			h++;
		}
		else
		{
			b[i] = a[j];
			j++;
		}
		i++;
	}
	if (m < h)
	{
		for (k = j; k <= r; k++)
		{
			b[i] = a[k];
			i++;
		}
	}
	else
	{
		for (k = h; k <= m; k++)
		{
			b[i] = a[k];
			i++;
		}
	}
	for (k = l; k <= r; k++)
	{
		a[k] = b[k];
	}

}

void Sort(int* a, int* b, int l, int r)
{
	int m;
	if (l < r)
	{
		m = (l + r) / 2;
		Sort(a, b, l, m);
		Sort(a, b, (m + 1), r);
		Merge(a, b, l, m, r);
	}
}


int main()
{	
	double Time_Start = 0, Time_End = 0;
	int Size_M = 0;
	cout << "\n\tMerge sort realization (one-flow)\n\tTimofeev E.V. 381708 - 2.\n\n" << "Enter the size of massive: ";
	cin >> Size_M;
	if (Size_M<=0)
	{
		cout << "\n\n\tInvalid input!\nSize of massive must be greather than zero!\n\n";
		system("pause");
		return 0;
	}
	int* Unsorted_Massive = new int[Size_M];
	int* Part_Sorted_Massive = new int[Size_M];

	srand(time(NULL));
	cout << "\n\nGenerated unsorted massive: ";
	for (int i = 0; i < Size_M; i++)
	{
		Unsorted_Massive[i] = rand() % Size_M;
		cout << Unsorted_Massive[i] << "|";
	}
	cout << "\n\n";

	Time_Start = clock();
	Sort(Unsorted_Massive, Part_Sorted_Massive, 0, Size_M-1);
	Time_End = clock();
	
	cout << "Sorted massive: ";
	for (int i = 0; i < Size_M; i++)
	{
		cout << Unsorted_Massive[i] << "|";
	}
	cout << "\n\n";
	cout << "Total time = " << (Time_End - Time_Start) / CLK_TCK << " seconds\n\n";
}

