// Copyright 2020 Utkin Andrey

#pragma once

#include <vector>

// Получить конкретный байт из числа
uint8_t getByte(const int& value, const uint8_t index);

// Сгенерировать гистограмы для поразрядной сортировки
void genHistograms(std::vector<int>& hist, std::vector<int>& a,
                   uint8_t byteInd);
// Сгенерировать отступы по разрядам
void genOffsets(std::vector<int>& offsets, const std::vector<int>& hist);
// Чётно-нечётно слияние в буфер
void oddEvenMerge(const std::vector<int>& a, const std::vector<int>& b,
                  std::vector<int>& buff);

std::vector<int> randomVector(const size_t size);

// Поразрядная сортировка массива(1 разряд = 1 байту)
void radixSort(std::vector<int>& a);

void sortParll(std::vector<int>& a);
