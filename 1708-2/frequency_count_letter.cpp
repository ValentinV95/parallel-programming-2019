// Copyright 2019 Streltsov Roman
#include <mpi.h>
#include <random>
#include <cstring>
#include <iostream>
#include "../../../modules/task_1/streltsov_r_frequency_count_letter/frequency_count_letter.h"

void getRandomStr(char *word, int wordSize) {
    const char ch_array[] = "abcdefghijklmnopqrstuvwxyz";
    if (wordSize < 1)
        throw "Error size str";
    for (int i = 0; i < wordSize; i++) {
        word[i] = ch_array[rand() % (strlen(ch_array) - 1)];
    }

    word[wordSize] = 0;
}

int getCountFreqLettInWord(char *word, char letter) {
    int count = 0;
    int length_word = strlen(word);
    for (int i = 0; i < length_word; i++) {
        if (word[i] == letter)
            count++;
    }
    return count;
}

int getCountParallelFreqLettInWord(char* word, char letter) {
    int proc_size, proc_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &proc_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

    int delta = strlen(word) / proc_size;
    int delta_remainder = strlen(word) % proc_size;

    int global_count = 0;
    int local_count = 0;

    char* word_local = new char[delta + 1];
    if (proc_rank == 0) {
        delete[] word_local;
        word_local = new char[delta + delta_remainder + 1];
    }

    char letter_local;

    if (proc_rank == 0) {
        for (int proc = 1; proc < proc_size; proc++) {
            MPI_Send(&letter, 1, MPI_CHAR, proc, 0, MPI_COMM_WORLD);
            MPI_Send(&word[0] + proc * delta + delta_remainder, delta, MPI_CHAR, proc, 0, MPI_COMM_WORLD);
        }
    }

    if (proc_rank == 0) {
        for (int i = 0; i < delta + delta_remainder; i++) {
            word_local[i] = word[i];
        }
        word_local[delta + delta_remainder] = 0;
        letter_local = letter;
    } else {
        MPI_Status status;
        MPI_Recv(&letter_local, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&word_local[0], delta, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
        word_local[delta] = 0;
    }

    local_count = getCountFreqLettInWord(word_local, letter_local);

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    delete[] word_local;

    return global_count;
}
