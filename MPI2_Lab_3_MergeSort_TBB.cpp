#include <tbb/tbb.h>
#include <ctime>
#include <iostream>
using namespace tbb;
using namespace std;


void Sort(int* Massive, int Start, int End) 
{
    int ST = Start, EN = End;
    int Middle = Massive[(Start + End) / 2];
    for (int f = ST; f < EN; f++)
    {
        for (int s = ST; s < End; s++)
        {
            if (Massive[s] >= Middle)
                break;
            else
                ST++;
        }
        for (int s = EN; s > Start; s--)
        {
            if (Massive[s] <= Middle)
                break;
            else
                EN--;
        }
        if (ST <= EN)
        {            
            int Tmp_1 = Massive[ST];
            int Tmp_2 = Massive[EN];
            Massive[ST] = Tmp_2;
            Massive[EN] = Tmp_1;
            ST++;
            EN--;
        }
        if (ST > EN)
            break;
    }
    if (ST < End)
        Sort(Massive, ST, End);
    if (Start < EN)
        Sort(Massive, Start, EN);
}


void Merge(int* Massive, int Temp_Size_1, int Temp_Size_2, int Start_1, int Start_2) 
{
    int i = 0, j = 0, k = 0;
    int* Merge_Massive = new int[Temp_Size_1 + Temp_Size_2];
    while (i < Temp_Size_1 && j < Temp_Size_2) 
    {
        if (Massive[Start_1 + i] <=Massive[Start_2 + j]) 
        {
            Merge_Massive[k] = Massive[Start_1 + i];
            i++;
        }
        else 
        {
            Merge_Massive[k] = Massive[Start_2 + j];
            j++;
        }
        k++;
    }
    if (i < Temp_Size_1)
    {
        for (int p = i; p < Temp_Size_1; p++) 
        {
            Merge_Massive[k] = Massive[Start_1 + p];
            k++;
        }
    }
    else 
    {     
        for (int p = j; p < Temp_Size_2; p++) 
        {
            Merge_Massive[k] = Massive[Start_2 + p];
            k++;
        }
    }
    for (i = Start_1;i < Start_2 + Temp_Size_2; i++) 
        Massive[i] = Merge_Massive[i - Start_1];
}

void Parallel_Sort(int* Massive, const int Size_M, const int Proc_N) 
{
    task_scheduler_init init(Proc_N);
    int Temp_Massive = Size_M / Proc_N;
    parallel_for(blocked_range<int>(0, Proc_N),[=, &Massive](blocked_range<int>&ASC) 
    {
        for (int i = ASC.begin(); i < ASC.end(); i++) 
        {
            int Start = i * Temp_Massive;
            int End = 0;
            if (i == Proc_N - 1 && Size_M % Proc_N) 
                End = Size_M - 1;
            else 
                End = Start + Temp_Massive - 1;
            Sort(Massive, Start, End);
        }
    });
    int Distance = 1; 
    for (int i = Proc_N / 2; i > 0; i /= 2) 
    {
        int Size_1 = Temp_Massive * Distance;
        int Size_2 = Size_1;
        if (i / 2 <= 0) 
            Size_2 += Size_M % Proc_N;
        parallel_for(blocked_range<int>(0, i),[=, &Massive](blocked_range<int>&ASC) 
        {
            for (int j = ASC.begin(); j < ASC.end(); j++) {
                int Index_1 = j * Temp_Massive * Distance * 2;
                int Index_2 = Index_1 + Temp_Massive * Distance;
                Merge(Massive, Size_1, Size_2, Index_1, Index_2);
            }
        });
        Distance *= 2;
    }
    init.terminate();
}

int main(int argc, char** argv) 
{
    srand(time(NULL));
    int Size_M, Proc_N;
    cout << "\n\tMerge sort realization with TBB\n\tTimofeev E.V. 381708 - 2.\n\n" << "Enter the number of threads: ";
    cin >> Proc_N;
    if (Proc_N <= 0)
    {
        cout << "\n\n\tInvalid input!\nValue of threads must be greather than zero!\n\n";
        system("pause");
        return 0;
    }
    cout << "Enter the size of unsorted massive: ";
    cin >> Size_M;
    if (Size_M <= 0 || Size_M < 2)
    {
        cout << "\n\n\tInvalid input!\nSize of unsorted massive must be greather than zero and two!\n\n";
        system("pause");
        return 0;
    }
   
    int* Massive = new int[Size_M];   
    cout << "\n\n";
    cout << "\tTotal threads = " << Proc_N << "\n\nGenerated unsorted massive: ";
    for (int i = 0; i < Size_M; i++)
    {
        Massive[i] = rand() % Size_M;
        cout << Massive[i] << "|";
    }  
    tick_count WTime_Start = tick_count:: now();
    Parallel_Sort(Massive, Size_M, Proc_N);
    tick_count WTime_End = tick_count::now();
    cout << "\n\nSorted massive:\t\t    "; 
    for (int i = 0; i < Size_M; i++)
    {
        cout << Massive[i] << "|";
    }

    cout << "\n\nParallel time: " << (WTime_End - WTime_Start).seconds() << " seconds.\n\n";
    return 0;
}

