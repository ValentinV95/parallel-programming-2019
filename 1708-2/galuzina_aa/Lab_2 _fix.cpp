#include <omp.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <vector>
using namespace std;

void Even_Odd_Merge(vector<int>* Massive) 
{
    const int LEN = Massive->size();
    int TS = static_cast<int>(ceil(log2(LEN))), PS = static_cast<int>(pow(2, TS - 1));
    while (PS > 0) 
    {
        int q = static_cast<int>(pow(2, TS - 1)), r = 0, d = PS;        
        while (d > 0) 
        {
            #pragma omp parallel for  shared(Massive)
            for (int i = 0; i < LEN - d; ++i)
            {
                if ((i & PS) == r) 
                {
                    if (Massive->at(i) > Massive->at(i + d))
                        iter_swap(Massive->begin() + i, Massive->begin() + i + d);
                }
            }
            d = q - PS;
            q /= 2;
            r = PS;
        }
        PS /= 2;
    }
}


int STEP(int ITR) 
{
    int ST = 0;
    if (ITR % 2)
        ST = static_cast<int>(8 * pow(2, ITR) - 6 * pow(2, (ITR + 1) / 2) + 1);
    else 
        ST = static_cast<int>(9 * pow(2, ITR) - 9 * pow(2, ITR / 2) + 1);
    return ST;
}

void Batcher(vector<int>* Massive, const int ST) 
{
    vector<int>* Middle = new vector<int>;
    #pragma omp for
    for (int STR = 0; STR < ST; STR++)
    {
        for (unsigned int i = STR, j = 0; i < Massive->size(); i += ST, j++)
            Middle->push_back(Massive->at(i));
        Even_Odd_Merge(Middle);
        unsigned int i = STR, j = 0;
        for (; i < Massive->size() - STR; i += ST, j++)
            Massive->at(i) = Middle->at(j);
        Middle->clear();
    }
}

void Shell_sort(vector<int>* Massive, int Size_M) 
{
    int ST = 0, ITR = 0;
    while (STEP(ITR++) < Size_M / 3)
        ST = STEP(ITR);
    while (--ITR >= 0)
    {
        ST = STEP(ITR);
        Batcher(Massive, ST);
    }
}

int main() 
{
    srand(time(NULL));
    setlocale(LC_ALL, "Russian");
    double Time_Start = 0, Time_End = 0;
    int Size_M = 0, Proc_N = 0;
    cout << "\n\tСортировка Шелла с четно-нечетным слиянием Бэтчера\n\tГалузина Анна. 381708 - 2.\n\n" << "Введите размер массива: ";
    cin >> Size_M;
    if (Size_M <= 0 || Size_M < 2)
    {
        cout << "\n\n\tНекорректный ввод!\nРазмер массива должен быть больше нуля и одного!\n\n";
        system("pause");
        return 0;
    }
    cout << "\n\nВведите количество потоков: ";
    cin >> Proc_N;
    if (Proc_N <= 0)
    {
        cout << "\n\n\tНекорректный ввод!\nКоличество потоков должно быть больше нуля!\n\n";
        system("pause");
        return 0;
    }
    
    vector<int>* Massive = new vector<int>(Size_M);
    cout << "\n\nСгенерированный неотсортированный массив: ";
    for (int i = 0; i < Size_M; i++)
    {
        Massive->at(i) = rand() % Size_M;
        if (i < 100)
            cout << Massive->at(i) << " ";
    }
    Time_Start = omp_get_wtime();
    omp_set_num_threads(Proc_N);
    Shell_sort(Massive, Size_M);
    Time_End = omp_get_wtime();

    cout << "\n\nОтсортированный массив:\t\t\t  ";
    for (int i = 0; i < Size_M; i++)
    {      
        if (i < 100)
            cout << Massive->at(i) << " ";
    }
    cout << "\n\nВремя обработки составило: " << Time_End - Time_Start << " секунд.\n\n";
    system("pause");
    return 0;
}