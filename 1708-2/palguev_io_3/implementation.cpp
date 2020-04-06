#include <mpi.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include "implementation.h"

namespace functions
{
	double get_max(double input, std::vector<double> intervals,
		std::vector<double> points)
	{
		auto max = 0.0;

		for (auto i = 1; i < static_cast<int>(intervals.size()); i++)
		{
			auto temp = (double)(fabs(points[i] - points[i - 1])) /
				(double)(intervals[i] - intervals[i - 1]);
			max = std::max(temp, max);
		}

		return max > 0.0 ? input * max : 1.0;
	}

	double get_interval_right(double val1, double val2, double x_lleft, double x_right,
		double f_x_left, double f_x_right, double max)
	{
		const auto delta = x_right - x_lleft;

		if (val1 == x_lleft)
		{
			return 2 * (delta)-4 *
				((double)(f_x_right) / (double)(max));
		}

		if (val2 == x_right)
		{
			return 2 * (delta)-4 *
				((double)(f_x_left) / (double)(max));
		}

		return x_right - x_lleft +
			(double)(std::pow(f_x_right - f_x_left, 2)) /
			(double)(std::pow(max, 2) * delta) - 2 *
			((double)(f_x_right + f_x_left) / (double)(max));
	}

	double Impl::get_point(double x_left, double x_right, double f_x_left,
		double f_x_r, double max, int max_num, int size)
	{
		if (max_num == 1 || max_num == size)
		{
			return (double)(x_right + x_left) / 2.0;
		}

		return ((double)(x_right + x_left) / 2.0) -
			((double)(f_x_r - f_x_left) / (2.0 * max));
	}

	void InitializePoints(double a, double b, int size, std::vector<double>& points)
	{
		auto range = (double)(b - a) / (double)(size + 1);

		points[0] = a;

		for (auto i = 0; i < size; i++)
		{
			points[i + 1] = a + (i + 1) * range;
		}

		points[size + 1] = b;
	}

	void GetM(double r, double(*func)(double), std::vector<double>& f_points, double& M, std::vector<double>& points)
	{
		f_points.resize(points.size());
		for (auto i = 0; i < (int)(f_points.size()); i++)
		{
			f_points[i] = func(points[i]);
		}

		M = get_max(r, points, f_points);
	}

	void PrepareMaxIntervalsVector(int size, std::vector<int>& m_intervals, std::vector<double>& d_vec, int iterationNumber)
	{
		double minElem;
		for (auto indexI = 0; indexI < size + iterationNumber; indexI++)
		{
			minElem = *std::min_element(m_intervals.begin(),
				m_intervals.end());

			for (auto indexJ = 0; indexJ < size; indexJ++)
			{
				if (m_intervals[indexJ] == minElem &&
					(d_vec[indexI] > m_intervals[indexJ] || m_intervals[indexJ] == 0.0))
				{
					m_intervals[indexJ] = indexI + 1;
					break;
				}
			}
		}

		std::sort(m_intervals.begin(), m_intervals.end());
	}

	void Recv(int* max_num)
	{
		MPI_Status status;
		MPI_Recv(max_num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
			&status);
	}

	void Recv(double* max_num)
	{
		MPI_Status status;
		MPI_Recv(max_num, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD,
			&status);
	}

	std::vector<double> Impl::get_result(double a, double b, double r, double error,
		double (*func)(double)) {
		if (b <= a)
		{
			throw "incorrect input arguments";
		}

		int size, rank;
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);

		std::vector<double> recv_points(size);
		std::vector<int> m_intervals(size);
		std::vector<double> f_points;
		std::vector<double> d_vec;
		auto length = 2;

		std::vector<double> points(size + length);
		if (rank == 0)
		{
			InitializePoints(a, b, size, points);
		}

		double M;
		auto iterationNumber = 1;
		double result_point = 0.0;
		double result_func_point = 0.0;

		while (true)
		{
			if (rank == 0)
			{
				GetM(r, func, f_points, M, points);
			}

			MPI_Bcast(&M, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

			double curr_error;
			if (rank == 0)
			{
				d_vec.resize(size + iterationNumber);
				for (auto i = 0; i < size + iterationNumber; i++)
				{
					d_vec[i] = get_interval_right(a, b,
						points[i], points[i + 1],
						f_points[i], f_points[i + 1],
						M);
				}

				for (auto i = 0; i < size; i++)
				{
					m_intervals[i] = 0.0;
				}

				PrepareMaxIntervalsVector(size, m_intervals, d_vec, iterationNumber);

				curr_error = 0.0;
				for (auto i = 0; i < size; i++)
				{
					auto value = points[m_intervals[i]] -
						points[m_intervals[i] - 1];
					if (value > curr_error)
					{
						curr_error = value;
						result_point = points[m_intervals[i]];
						result_func_point = f_points[m_intervals[i]];
					}
				}
			}

			MPI_Bcast(&curr_error, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			if (curr_error <= error) break;

			int max_num;

			if (rank == 0)
			{
				for (auto index = 1; index < size; index++)
				{
					MPI_Send(&m_intervals[0] + index, 1, MPI_INT, index, 0,
						MPI_COMM_WORLD);
				}
			}

			if (rank == 0)
			{
				max_num = m_intervals[0];
			}
			else
			{
				Recv(&max_num);
			}

			if (rank == 0)
			{
				for (auto index = 1; index < size; index++)
				{
					MPI_Send(&points[0] + m_intervals[index] - 1, length,
						MPI_DOUBLE, index, 0, MPI_COMM_WORLD);
				}
			}

			std::vector<double> range(length);

			if (rank == 0)
			{
				for (auto i = 0; i < length; i++)
				{
					range[i] = points[max_num - 1 + i];
				}
			}
			else
			{
				Recv(&range[0]);
			}

			if (rank == 0)
			{
				for (auto index = 1; index < size; index++)
				{
					MPI_Send(&f_points[0] + m_intervals[index] - 1, length,
						MPI_DOUBLE, index, 0, MPI_COMM_WORLD);
				}
			}

			std::vector<double>
			f_range(length);

			if (rank == 0)
			{
				for (auto index = 0; index < length; index++)
				{
					f_range[index] = f_points[max_num - 1 + index];
				}
			}
			else
			{
				Recv(&f_range[0]);
			}

			auto point = get_point(range[0], range[1],
				f_range[0],
				f_range[1],
				M, max_num, size + iterationNumber);

			MPI_Gather(&point, 1, MPI_DOUBLE, &recv_points[0], 1, MPI_DOUBLE,
				0, MPI_COMM_WORLD);

			if (rank == 0)
			{
				for (auto i = 0; i < size; i++)
				{
					points.emplace_back(recv_points[i]);
				}
				std::sort(points.begin(), points.end());
				iterationNumber++;
			}
		}

		std::vector<double> result(length);
		if (rank == 0)
		{
			result = { result_func_point, result_point };
		}

		MPI_Bcast(&result[0], length, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		return result;
	}
}