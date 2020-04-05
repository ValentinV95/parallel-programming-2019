// Copyright Utkin Andrey 2020

#pragma once

#include <string>

// Получить строку, содержащюю определнное кол-во слов
std::string getRandomString(const int words);

// Получить кол-во слов в строке
size_t wordCount(const std::string& string);
// Получить кол-во слов в строке(паралл. алгоритм)
size_t wordCount_p(std::string& string);
