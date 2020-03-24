#include <iostream>
using namespace std;

int i, j, n, d, count;

template <class T>
void Shell(T& a, T&b, int n) //сортировка Шелла
{
d=n;
d=d/2;
while (d>0)
{
for (i=0; i<n-d; i++)
{
j=i;
while (j>=0 && a>b)
{
a=b;
j--;
}
}

template <class T>
void shuffle(std::vector<T>& a, unsigned int l, unsigned int r)
{
    auto half = (unsigned int) (l + r) / 2;
    std::vector<T> tmp(a.size());
    unsigned int i, j;
    for (i = l, j = 0; i <= r; i += 2, j++)
    {
        tmp[i] = a[l + j];
        tmp[i + 1] = a[half + j + 1];
    }
    for (auto i = 0; i < tmp.size(); i++)
       a[i] = tmp[i];
}

template <class T>
void unshuffle(std::vector<T>& a, unsigned int l, unsigned int r)
{
    auto half = (unsigned int) (l + r) / 2;
    std::vector<T> tmp(a.size());
    unsigned int i, j;
    for (i = l, j =0; i<=r; i += 2, j++)
    {
        tmp[l + j] = a[i];
        tmp[half + j + 1] = a[i + 1];
    }
    for (auto i = 0; i < tmp.size(); i++)
        a[i]  = tmp[i];
}

template <class T>
void OddEvenMergeSort(std::vector<T>& a, unsigned int l, unsigned int r)
{
    if (r == l + 1) Shell(a[l], a[r], l); //мы дошли до подмассива размера 2
    if (r < l + 2) return; //дошли до подмассива размера 1 - выходим, такой подмассив априори отсортирован
    unshuffle(a, l, r); //делим подмассив на две части
    auto half = (unsigned int) (l + r) / 2;
    OddEvenMergeSort(a, l, half);
    OddEvenMergeSort(a, half + 1, r); //вызываемся рекурсивно для половинок
    shuffle(a, l, r); //сливаем части
    for (auto i = l + 1; i < r; i += 2)
        Shell(a[i], a[i + 1]);
    auto halfSize = (r - l + 1) / 2 - 1;       //*
    for (int i = l + 1; i + halfSize < r; i++) //*
        Shell(a[i], a[i + halfSize]);    //*
}