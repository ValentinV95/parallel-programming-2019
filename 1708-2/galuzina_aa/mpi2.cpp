#include "mpi.h"
#include <vector>
#include <string>
#include <random>
#include <stdlib.h>
#include <ctime>
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <ctime>
#include <cmath>
#include <string>
#include <Windows.h>

using namespace std;

int getPositive_elem(std::vector<int> vector, int count_size_vector) {
	if (count_size_vector < 2) {
		return 0;
	}
	int negative_elem = 0;
	for (int c = 0; c < count_size_vector; c++) {
		if (vector[c] == -1) {
			negative_elem++;
		}
	}
	return vector.size() - negative_elem;
}
int* Create_dinamic_massiv_from_vector(std::vector<int> vec) {//принимает вектор, возвращает массив
	int vec_size = vec.size(); //записыавем размер вектора
	int* mas = new int[vec_size];//создаем массив размера это вектора
	for (int i = 0; i < vec_size; i++) {
		mas[i] = vec[i];
	}
	return mas;
}

int Consumer(int *buffer, int buffer_size, int rank_proc, int* resurce) {
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (size == 1 || rank_proc == 0) { // если 0 процес выполнет функцию или кол-во проц = 1
		for (int i = 0; i < buffer_size; i++) {
			if (buffer[i] != -1) {
				*resurce = buffer[i];
				buffer[i] = -1;
				break;
			}
		}
	}
	else {
		if (rank == 0) {
			int temp_resurs;
			for (int i = 0; i < buffer_size; i++) {
				if (buffer[i] != -1) {
					temp_resurs = buffer[i];
					buffer[i] = -1;
					break;
				}
			}
			MPI_Send(&temp_resurs, 1, MPI_INT, rank_proc, 0, MPI_COMM_WORLD); //отправляет ресурс процессору который его вызвал
		}
		else {
			if (rank == rank_proc) {
				int temp_resurs;
				MPI_Recv(&temp_resurs, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);//принимает данные от производителя на 0
				*resurce = temp_resurs; // отправленный ресурс делает принятым 
			}
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);
	return 0;
}
int Producer(int *buffer, int buffer_size, int rank_proc, int resurce) {
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (size == 1 || rank_proc == 0) {
		for (int i = 0; i < buffer_size; i++) {
			if (buffer[i] == -1) {
				buffer[i] = resurce; //заполнение буфера ресурсом
				break;
			}
		}
	}
	else {
		if (rank == 0) { //если функцию вызвал 0 процессор, то принимает
			int resurce_for_bufer;
			MPI_Recv(&resurce_for_bufer, 1, MPI_INT, rank_proc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //прием сообщения процессора вызывающего функцию производителей
			for (int i = 0; i < buffer_size; i++) { //по кол-ву эл буфера заполнение буфера ресурсом
				if (buffer[i] == -1) {
					buffer[i] = resurce;
					break;
				}
			}
		}
		else { //если любой другой, то отправка нулевому 
			if (rank == rank_proc) {
				MPI_Send(&resurce, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			}
		}
	}
	MPI_Barrier(MPI_COMM_WORLD); //Блокирует звонящего до тех пор, пока все члены группы не вызовут его; вызов возвращается в любом процессе только после того, как все члены группы вошли в вызов.
	return 0;
}
int main(int argc, char** argv)
{
	setlocale(LC_ALL, "Russian");
	int rank, size;
	double st1, st2, et1, et2;  //time
	MPI_Init(&argc, &argv); //принимает данные с консоли иницализирует работы 
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // возвращает ранг процессоров 
	MPI_Comm_size(MPI_COMM_WORLD, &size); //количество
	int kol_elem_in_buffer = 100;
	int *buffer = new int[kol_elem_in_buffer];
	for (int i = 0; i < kol_elem_in_buffer; i++) {
		buffer[i] = -1;
	}
	int kol_resursov = atoi(argv[1]); //задаем кол-во ресурсов с консоли
	if (size == 1) {
		st2 = MPI_Wtime();
		for (int i = 0; i < kol_resursov; i++) {
			Producer(buffer, kol_elem_in_buffer, rank, i);
		}
		std::vector<int> resurce_consume1(kol_resursov, -1); //заполнение -1
		int *resurce_consume;
		resurce_consume = Create_dinamic_massiv_from_vector(resurce_consume1); //вернула массив
		for (int i = 0; i < kol_resursov; i++) {
			Consumer(buffer, kol_elem_in_buffer, rank, &resurce_consume[i]); //отправляю данные в массиве 
			et2 = MPI_Wtime();
			cout << resurce_consume[i] << "\n" << "Time_Single" << st2-et2 << std::fixed << " sec \n";;
		}
	}
	else { // многопроцесорная 
		st1 = MPI_Wtime();
		for (int i = 0; i < kol_resursov; i++) {
			Producer(buffer, kol_elem_in_buffer, 1, i); //отправляем по кол-ву ресурсов отправляем кол-во эл в буфере 
		}
		std::vector<int> resurce_consume1(kol_resursov, -1);
		int *resurce_consume;
		resurce_consume = Create_dinamic_massiv_from_vector(resurce_consume1);
		for (int i = 0; i < kol_resursov; i++) {
			Consumer(buffer, kol_elem_in_buffer, 1, &resurce_consume[i]); //отправляю -1 (массив)
		}
		if (rank == 1) { //выписваю ресурсы если ранг 1
			for (int i = 0; i < kol_resursov; i++) {
				et1 = MPI_Wtime();
				cout << resurce_consume[i] << "\n" << "Time_Multyply" << st1 - et1 << std::fixed << " sec \n";;
			}
		}
	}
	MPI_Finalize();
	return 0;
}
