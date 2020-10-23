#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <random>
#include <iostream>
#include  <time.h> 
#include <tbb/tbb.h>

size_t grainsize = 100;

std::vector<int> GetRandVector(int size) {
  if (size < 1)
    throw - 1;
  std::mt19937 gen;
  std::vector<int> vec(size);
  for (int i = 0; i < size; i++) {
    vec[i] = gen() % 100;
  }
  return vec;
}

void check(int& a, int& b) {
  if (a > b) std::swap(a, b);
}

void CoutMass(std::vector<int>& mas) {
  for (int i = 0; i < mas.size(); i++) {
    std::cout << " " << mas[i] << " ";
  }
  std::cout << "\n";
}


void merger(std::vector<int>& mas, int first, int last) {
  int half = (unsigned int)(first + ((last - first) / 2));
  std::vector<int> Temp(mas.size());
  int jleft = first;
  int  jright = half + 1;
  tbb::blocked_range<int> range(first, last, grainsize);
  for (int i = range.begin(); i <= range.end(); i++)
  {

    if ((jright > last) || (jleft < (half + 1)) && (mas[jleft] < mas[jright]) && (jleft < (first + half + 1))) {
      Temp[i] = mas[jleft];
      jleft++;
    }
    else if (jright <= (last)) {
      Temp[i] = mas[jright];
      jright++;
    }
  }
  tbb::blocked_range<int>;
  for (int i = range.begin(); i <= range.end(); i++)
  {
      mas[i] = Temp[i];
  }
}


void unmerger(std::vector<int>& mas, int first, int last) {
  int half = (unsigned int)(first + ((last - first) / 2));
  int nach = first;
  std::vector<int> Temp(mas.size());
  int jleft = first;
  int jright = half + 1;
  if (first % 2 == 0){
    tbb::blocked_range<int> range(first, last, grainsize);
    for (int i = range.begin(); i <= range.end(); i++)
    {
        if (i % 2 == 0) {
          Temp[jleft] = mas[i];
          jleft++;
        }
        else {
          Temp[jright] = mas[i];
          jright++;
        }
    }
  }
  else {
    tbb::blocked_range<int> range(first, last, grainsize);
    for (int i = range.begin(); i <= range.end(); i++)
    {
      if (i % 2 == 0) {
        Temp[jright] = mas[i];
        jright++;
      }
      else {
        Temp[jleft] = mas[i];
        jleft++;
      }
    }
  }

  tbb::blocked_range<int> range(first, last, grainsize);
  for (int i = range.begin(); i <= range.end(); i++)
  {
    mas[i] = Temp[i];
  }
}

void BatcherSort(std::vector<int>& mas, int first, int last) {
  if (first == last - 1) {
    check(mas[first], mas[last]);//сравнение элементов
    return;
  }
  if (last < first + 2) return;

  unmerger(mas, first, last);//Разделяем массив
  int half = (unsigned int)(first + ((last - first) / 2));
  BatcherSort(mas, first, half);
  BatcherSort(mas, half + 1, last); //Рекурсия вызов для каждой части
  merger(mas, first, last); //Совмещаем части
  tbb::blocked_range<int> range(first, last, grainsize);
  for (int i = range.begin(); i <= range.end(); i+=2)
  {
    if ((i + 1) > last) break;
    check(mas[i], mas[i + 1]);
  };
  int halfSize = (unsigned int)((last - first + 1) / 2 - 1);
  tbb::blocked_range<int>;
  for (int i = range.begin(); i + halfSize <= range.end(); i++)
  {
      check(mas[i], mas[i + halfSize]);
  }
}

int main() {
  int Len;
  std::cout << "Lenght: ";
  std::cin >> Len;
  std::cout << "\n";


  std::vector<int> mas = GetRandVector(Len);
  //std::cout << "Before: "; 
  //CoutMass(mas);
  //std::vector<int> mas1 = mas;
  clock_t t1 = std::clock();
  BatcherSort(mas, 0, Len - 1);
  clock_t t2 = std::clock();
  //std::sort(mas1.begin(), mas1.end());
  //std::cout << "After: ";
  //CoutMass(mas);
  //bool flag = 1;
  //for (int i = 0; i < Len - 1; i++) {
  //  if (mas[i] != mas1[i]) {
  //    flag = 0;
  //    std::cout << "NERABOTAYET";
  //    break;
  //  }
  //}
  //if (flag == 1) std::cout << "RABOTAET\n\n";
  std::cout <<" TIME: "<< t2 - t1;
}