
#include "pch.h"
#include <stdlib.h>
#include <cmath>
#include <iostream>

using namespace std;


float f(float x) {

	return  x * x / (x + 1) * (2 * x + 1);
		

}

float rect(float local_a, float local_b, float local_n, float(*f)(float))
{
	float local_integral;
	float x;
	int i;
	float local_h;
	


	local_h = (local_b - local_a) / local_n;
	local_integral = f(local_a);
	x = local_a;
	for (i = 1; i <= local_n - 1; i++)
	{
		x = x + local_h;
		local_integral += f(x);
	}
	local_integral *= local_h;

	return local_integral;
}

int main()
{

	// Integral of rect of x^0 over (1.0,2.0);

	cout << rect<<1.0, 2.0, 2054;
	cout << f;

	return 0;
}