#include "pch.h"
#include<stdio.h>
#include<mpi.h>
#include<time.h>

#define M 320
#define N 40
#define NUM_DIMS 1

#define EL(x) (sizeof(x) / sizeof(x[0][0]))

static double A[N][M], B[M][N], C[N][M];

int main(int argc, char **argv)

{
	int        rank, size, i, j, k, i1, j1, d, sour, dest;

	int        dims[NUM_DIMS], periods[NUM_DIMS], new_coords[NUM_DIMS];

	int        reorder = 0;

	MPI_Comm   comm_cart;

	MPI_Status st;

	int dt1;

	/* Инициализация библиотеки MPI*/

	MPI_Init(&argc, &argv);

	/* Каждая ветвь узнает количество задач в стартовавшем приложении */

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	/* и свой собственный номер: от 0 до (size-1) */

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/* Обнуляем массив dims и заполняем массив periods для топологии "кольцо" */

	for (i = 0; i < NUM_DIMS; i++) { dims[i] = 0; periods[i] = 1; }

	/* Заполняем массив dims, где указываются размеры (одномерной) решетки */

	MPI_Dims_create(size, NUM_DIMS, dims);

	/* Создаем топологию "кольцо" с communicator(ом) comm_cart */

	MPI_Cart_create(MPI_COMM_WORLD, NUM_DIMS, dims, periods, reorder,

		&comm_cart);

	/* Отображаем ранги на координаты компьютеров, с целью оптимизации

	 * отображения заданой виртуальной топологии на физическую топологию

	 * системы. */

	MPI_Cart_coords(comm_cart, rank, NUM_DIMS, new_coords);

	/* Каждая ветвь находит своих соседей вдоль кольца, в направлении

	 * меньших значений рангов */

	MPI_Cart_shift(comm_cart, 0, -1, &sour, &dest);

	/* Каждая ветвь генерирует полосы исходных матриц A и B, полосы C обнуляет */

	for (i = 0; i < N; i++)

	{
		for (j = 0; j < M; j++)

		{
			A[i][j] = 3.141528;

			B[j][i] = 2.812;

			C[i][j] = 0.0;

		}

	}

	/* Засекаем начало умножения матриц */

	/* Каждая ветвь производит умножение своих полос матриц */

	/* Самый внешний цикл for(k) - цикл по компьютерам */

	for (k = 0; k < size; k++)

	{

		/* Каждая ветвь вычисляет координаты (вдоль строки) для результирующих

		 * элементов матрицы C, которые зависят от номера цикла k и

		 * ранга компьютера. */

		d = ((rank + k) % size)*N;

		/* Каждая ветвь производит умножение своей полосы матрицы A на

		 * текущую полосу матрицы B */

		for (j = 0; j < N; j++)

		{
			for (i1 = 0, j1 = d; j1 < d + N; j1++, i1++)

			{
				for (i = 0; i < M; i++)

					C[j][j1] += A[j][i] * B[i][i1];

			}

		}

		/* Умножение полосы строк матрицы A на полосу столбцов матрицы B в каждой

		 * ветви завершено */

		 /* Каждая ветвь передает своим соседним ветвям с меньшим рангом

		  * вертикальные полосы матрицы B. Т.е. полосы матрицы B сдвигаются вдоль

		  * кольца компьютеров */

		MPI_Sendrecv_replace(B, EL(B), MPI_DOUBLE, dest, 12, sour, 12,

			comm_cart, &st);

	}

	/* Умножение завершено. Каждая ветвь умножила свою полосу строк матрицы A на

	 * все полосы столбцов матрицы B.

	 * Засекаем время и результат печатаем */
	/* Для контроля печатаем первые четыре элемента первой строки результата */

	if (rank == 0)

	{
		for (i = 0; i < 1; i++)

			for (j = 0; j < 4; j++)

				printf("C[i][j] = %f\n", C[i][j]);

	}

	/* Все ветви завершают системные процессы, связанные с топологией comm_cart

	 * и завершаю выполнение программы */

	MPI_Comm_free(&comm_cart);

	MPI_Finalize();

	return(0);

}