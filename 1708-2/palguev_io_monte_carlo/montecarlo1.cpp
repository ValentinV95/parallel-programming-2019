// montecarlo1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

#include <math.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES



using namespace std;

void values(double  *p, double *q)
{
	double xmin = 0.0, xmax = 10.0;

	double x(0), y(0);
	x = (xmax - xmin) * (double)rand() /(double) RAND_MAX;
	y = (xmax - xmin) * (double)rand() /(double) RAND_MAX;
	if (sqrt((x*x) + (y*y)) <= 1)
	{
		(*p)++;
	}
	(*q)++;
	
}

double func(double x, double xmin, double xmax)
{
	

	if (x >= xmin && (0.7*x*x) - (0.1*x) <= xmax)
		return x * sqrt(1 + x);

}

int main()
{
	double x, y, j;
	int z, e;
	int c = 0;

	cin >> x >> y;
	cout << x << y << values << endl;


	cout << ("please enter the number\n") << endl;
	cin >> z;

	for (int i = 0; i < z; i++)
	{
		cout << values << x << y;
		if (y <= func(x, 0, +1))
			c++;
	}
	j = c / z;
	e = 10 * j;

	cout << "x number" << x << "y number" << y << "z number" << z << "c number is " << c
		<< "j number" << j << "e number" << e << endl;
	
}