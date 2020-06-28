#include <ctime>
#include <iostream>
using namespace std;

int main() 
{
    srand(time(NULL));
    setlocale(LC_ALL, "Russian");
    double Time_Start = 0, Time_End = 0;
    int Size_M = 0;
    cout << "\n\tСортировка Шелла с четно-нечетным слиянием Бэтчера\n\tГалузина Анна. 381708 - 2.\n\n" << "Введите размер массива: ";
    cin >> Size_M;
    if (Size_M <= 0 || Size_M < 2)
    {
        cout << "\n\n\tНекорректный ввод!\nРазмер массива должен быть больше нуля и одного!\n\n";
        system("pause");
        return 0;
    }

    int* Massive = new int[Size_M];
    cout << "\n\nСгенерированный неотсортированный массив: ";
    for (int i = 0; i < Size_M; i++)
    {
        Massive[i] = rand() % Size_M;
        cout << Massive[i] << "|";
    }
    cout << "\n\n";
    
    Time_Start = clock();
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
    Time_End = clock();

    cout << "Отсортированный массив:\t\t\t  ";
    for (int i = 0; i < Size_M; i++)
    {
        cout << Massive[i] << "|";
    }
    cout << "\n\nВремя обработки составило: " << (Time_End - Time_Start) / CLK_TCK << " секунд.\n\n";
    system("pause");
    return 0;
}