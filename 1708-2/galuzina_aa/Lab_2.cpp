#include <omp.h>
#include <ctime>
#include <iostream>

using namespace std;

void Shell_sort(int* Massive, int Size_M) {
    for (int i = Size_M / 2; i > 0; i /= 2)
    {
        for (int s = i; s < Size_M; s++)
        {
            int r;
            int a = Massive[s];
            for (r = s; r >= i; r -= i)
            {
                if (a < Massive[r - i])
                    Massive[r] = Massive[r - i];
                else
                    break;
            }
            Massive[r] = a;
        }
    }
}

void Splitter(int* Massive, int* Middle, int Size_S, int Size_E) {
    for (int i = 0; i < Size_S; i += 2)
        Middle[i] = Massive[i];
    int* Massive_Sec = Massive + Size_S;
    int c = 0, b = 0, s = 0; 
    while ((c < Size_S) && (b < Size_E)) 
    {
        if (Middle[c] <= Massive_Sec[b])
        {
            Massive[s] = Middle[c];
            c += 2;
        }
        else 
        {
            Massive[s] = Massive_Sec[b];
            b += 2;
        }
        s += 2;
    }
    if (c == Size_S) 
    {
        for (int j = b; j < Size_E; j += 2, s += 2)
            Massive[s] = Massive_Sec[j];
    }
    else 
    {
        for (int j = c; j < Size_S; j += 2, s += 2)
            Massive[s] = Middle[j];
    }
}

void Odd_Splitter(int* Massive, int* Middle, int Size_S, int Size_E) {
    for (int i = 1; i < Size_S; i += 2)
        Middle[i] = Massive[i];
    int* Massive_Sec = Massive + Size_S;
    int c = 1, b = 1, s = 1;
    while ((c < Size_S) && (b < Size_E)) 
    {
        if (Middle[c] <= Massive_Sec[b]) 
        {
            Massive[s] = Middle[c];
            c += 2;
        }
        else 
        {
            Massive[s] = Massive_Sec[b];
            b += 2;
        }
        s += 2;
    }
    if (c == Size_S) 
    {
        for (int j = b; j < Size_E; j += 2, s += 2)
            Massive[s] = Massive_Sec[j];
    }
    else 
    {
        for (int j = c; j < Size_S; j += 2, s += 2)
            Massive[s] = Middle[j];
    }
}

void Comparator(int* Massive, int Size_M) 
{
    for (int i = 1; i < (Size_M + 1) / 2; i++)
        if (Massive[2 * i] < Massive[2 * i - 1])
        {
            int Middle = Massive[2 * i - 1];
            Massive[2 * i - 1] = Massive[2 * i];
            Massive[2 * i] = Middle;
        }
}

void Parallel_sort(int* Massive, int Size_M, int Proc_N) 
{
    if (Proc_N == 1)
        Shell_sort(Massive, Size_M);
    else 
    {
        int* Middle = new int[Size_M];
        int Packet = Size_M / Proc_N;
        #pragma omp parallel shared(Massive, Size_M, Packet, Proc_N, Middle) num_threads(Proc_N)
        {
            int Current_Proc = omp_get_thread_num();
            int start = Current_Proc * Packet;
            Shell_sort(Massive + start, Packet);
            #pragma omp barrier
            int Batcher = Proc_N;
            int Size = Packet;
            while (Batcher != 1)
            {
                if (Current_Proc % 2 == 0 && Current_Proc < Batcher)
                    Splitter(Massive + Current_Proc * Size, Middle + Current_Proc * Size, Size, Size);
                if (Current_Proc % 2 == 1 && Current_Proc < Batcher)
                {
                    int t = Current_Proc - 1;
                    Odd_Splitter(Massive + t * Size, Middle + Current_Proc * Size, Size, Size);
                }
                #pragma omp barrier
                if (Current_Proc % 2 == 0 && Current_Proc < Batcher)
                    Comparator(Massive + Current_Proc * Size, Size * 2);
                #pragma omp barrier
                Batcher /= 2;
                Size *= 2;
            }
        }
        delete[] Middle;
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
    if (Proc_N <= 0 )
    {
        cout << "\n\n\tНекорректный ввод!\nКоличество потоков должно быть больше нуля!\n\n";
        system("pause");
        return 0;
    }

    int* Massive = new int[Size_M];
    cout << "\n\nСгенерированный неотсортированный массив: ";
    for (int i = 0; i < Size_M; i++)
    {
        Massive[i] = rand() % Size_M;
        cout << Massive[i] << " ";
        if (i == 100)
        {
            cout << " ... ";
            break;
        }
    }
    cout << "\n\n";
    Time_Start = omp_get_wtime();
    Parallel_sort(Massive, Size_M, Proc_N);
    Time_End = omp_get_wtime();
    cout << "Отсортированный массив:\t\t\t  ";
    for (int i = 0; i < Size_M; i++)
    {
        cout << Massive[i] << " ";
        if (i == 100)
        {
            cout << " ... ";
            break;
        }
    }
    cout << "\n\nВремя обработки составило: " << Time_End - Time_Start << " секунд.\n\n";
    system("pause");
    return 0;
}