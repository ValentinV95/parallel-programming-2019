#ifndef IMPLEMENTATION_H
#define IMPLEMENTATION_H
#include <vector>

namespace functions
{
	class Impl
	{
	public:
		static std::vector<double> get_result(double a, double b, double r, double min_error_bound,
			double(*func)(double));
		static double get_point(double x_left, double x_right, double f_x_left,
			double f_x_r, double max, int max_num, int size);		
	};
}
#endif // IMPLEMENTATION_H
