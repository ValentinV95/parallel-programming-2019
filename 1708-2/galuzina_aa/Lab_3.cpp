#include <tbb/tbb.h>
#include <math.h>
#include <stdio.h>
#include <ctime>
#include <time.h>
#include <utility>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace tbb;
using namespace std;



class OddEvenSorter 
{
    concurrent_vector<int>* Massive;
    const int p, r, d;
public:
        OddEvenSorter(concurrent_vector<int>* Massive, const int p, const int r, const int  d) : Massive(Massive), p(p), r(r), d(d) {}
        void operator()(const blocked_range<int>& range) const 
        {
            int STR = range.begin(), END = range.end();
            for (int i = STR; i != END; i++)
            {
                if ((p & i) == r) 
                {
                    if (Massive->at(i) > Massive->at(i + d))
                    {
                        int tmp = Massive->at(i);
                        Massive->at(i) = Massive->at(i + d);
                        Massive->at(i + d) = tmp;
                    }
                }
            }
    }
};

class BatcherSorter 
{
    concurrent_vector<int>* Massive;
    int ST;
public:
    BatcherSorter(concurrent_vector<int>* Massive, const int ST) : Massive(Massive), ST(ST) {}

    void operator()(const blocked_range<int>& range) const 
    {
        int STR = range.begin(), END = range.end();
        vector<int>* Middle = new vector<int>(Massive->size() / ST);
        for (int start = STR; start != END; start++) 
        {
            for (unsigned int i = start, j = 0; i < Massive->size(); i += ST, j++)
                Middle->push_back(Massive->at(i));
            const int length = Middle->size();
            int t = static_cast<int>(ceil(log2(length)));
            int p = static_cast<int>(pow(2, t - 1));
            while (p > 0) {
                int q = static_cast<int>(pow(2, t - 1));
                int r = 0;
                int d = p;
                int i;
                while (d > 0) {
                    for (i = 0; i < length - d; ++i) {
                        if ((i & p) == r) {
                            if (Middle->at(i) > Middle->at(i + d)) {
                                iter_swap(Middle->begin() + i,
                                    Middle->begin() + i + d);
                            }
                        }
                    }
                    d = q - p;
                    q /= 2;
                    r = p;
                }
                p /= 2;
            }

            unsigned int i = start, j = 0;
            for (; i < Massive->size() - start; i += ST, j++) {
                Massive->at(i) = Middle->at(j);
            }
            Middle->clear();
        }
        delete Middle;
    }
};


void Even_Odd_Merge(concurrent_vector<int>* Massive) 
{
    const int LEN = Massive->size();
    int TS = static_cast<int>(ceil(log2(LEN))), PS = static_cast<int>(pow(2, TS - 1));
    while (PS > 0)
    {
        int q = static_cast<int>(pow(2, TS - 1)), r = 0, d = PS;
        while (d > 0)
        {
            parallel_for(blocked_range<int>(1, LEN - d - 1, 40000), OddEvenSorter(Massive, PS, r, d));
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

void Batcher(concurrent_vector<int>* Massive, const int ST) 
{
    concurrent_vector<int>* Middle = new concurrent_vector<int>;
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

void Shell_sort(concurrent_vector<int>* Massive, int Size_M)
{
    int ST = 0, ITR = 0;
    while (STEP(ITR++) < Size_M / 3)
        ST = STEP(ITR);
    while (--ITR >= 0) 
    {
        ST = STEP(ITR);
        if (ST < 10) {
            Batcher(Massive, ST);
        }
        else {
            parallel_for(blocked_range<int>(0, ST, 100),
                BatcherSorter(Massive, ST));
        }
    }
}

int main() {

    srand(time(NULL));
    setlocale(LC_ALL, "Russian");
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

    concurrent_vector<int>*Massive = new concurrent_vector<int>(Size_M);
    cout << "\n\nСгенерированный неотсортированный массив: ";
    for (int i = 0; i < Size_M; i++)
    {
        Massive->at(i) = rand() % Size_M;
        if (i < 30)
            cout << Massive->at(i) << " ";
    }
    tick_count Time_Start = tick_count::now();
    task_scheduler_init init(Proc_N);
    Shell_sort(Massive, Size_M);
    tick_count Time_End = tick_count::now();

    cout << "\n\nОтсортированный массив:\t\t\t  ";
    for (int i = 0; i < Size_M; i++)
    {
        if (i < 30)
            cout << Massive->at(i) << " ";
    }
    cout << "\n\nВремя обработки составило: " << (Time_End - Time_Start).seconds() << " секунд.\n\n";
    system("pause");
    return 0;
    
    
}