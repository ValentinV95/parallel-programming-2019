// shellsortsequence.cpp : This file contains the 'main' function. Program execution begins and ends there.
//COPYRIGHT UTSHO_MOMEN

#include "pch.h"
#include <iostream>

using namespace std;
//a[] is sort , n is size
void ShellSort(int a[], int n)
{
	int gap;
	int j; 
	int i;
	int element;

	for (gap = n / 2; gap > 0; gap /= 2)
	{
		for (i = gap; i < n; i++)
		{
			element = a[i];
			for (j = i; j >= gap; j -= gap)
			{
				if (element < a[j - gap])
					a[j] = a[j - gap];
				else
					break;
			}
			a[j] = element;
		}
	}
}


void print(int a[], int size) 
{

	for (int i = 0; i < size; i++)
		cout << a[i] <<""; 
	cout << endl;
}

int main()
{
	int n;
	int a[20];

	cout << "enter element:" << endl;
	cin >> n; 
	cout << "enter elements the one that you put earlier:" << endl;
	for (int i = 0; i < n; i++) 
	{
		cin >> a[i];
	}
	cout << "array seq before sorting: ";
	print(a, n);
	ShellSort(a, n);
	cout << "array seq after sorting: ";
	print(a, n);
}



