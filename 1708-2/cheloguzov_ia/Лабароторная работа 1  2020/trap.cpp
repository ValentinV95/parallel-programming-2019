
#include "pch.h"
#include <time.h>
#include<stdio.h>
#include<math.h>
#include <stdlib.h>
#include <cmath>
#include <cassert>
#include <complex>
#include <iostream>
using namespace std;

                                        
double f(double x) {

	sqrt(x);
	return sqrt(x);
}



int main() {
	int n, i;
	double a, b, h, x, sum = 0, integral;
	                                     
	cout << "Enter the number of intervals: " << endl;;
	cin>> n;
	cout << "\nEnter the minimum limit: " << endl;;
	cin>>a;
	cout << "\nEnter the final limit: " << endl;;
	cin>>b;
	                                    
	h = fabs(b - a) / n;
	for (i = 1; i < n; i++) {
		x = a + i * h;
		sum = sum + f(x);
	}
	integral = (h / 2)*(f(a) + f(b) + 2 * sum);
	                                   
	
	cout<<"\n The integral is: " << integral;
}

