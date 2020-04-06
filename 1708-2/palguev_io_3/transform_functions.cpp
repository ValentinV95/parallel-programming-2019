#include <cmath>
#include "transform_functions.h"

double TransformFunction1(double input)
{
	return 100 + std::cos(input * 10);
}

double TransformFunction2(double input)
{
	return std::sqrt(input) * std::sin(input);
}

double TransformFunction3(double input)
{
	return std::sqrt(1 + 3 * std::pow(std::cos(input), 2)) + std::cos(10 * input);
}
