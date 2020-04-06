#pragma once
#include <vector>

double get_max(double input, std::vector<double> intervals,
               std::vector<double> points);
double get_interval_right(double val1, double val2, double x_lleft,
                          double x_right, double f_x_left, double f_x_right,
                          double max);
double get_point(double x_left, double x_right, double f_x_left,
                 double f_x_r, double max, int max_num, int size);
void InitializePoints(double a, double b, int size,
                      std::vector<double>& points);
void GetM(double r, double (* func)(double), std::vector<double>& f_points,
		  double& M, std::vector<double>& points);
void PrepareMaxIntervalsVector(int size, std::vector<int>& m_intervals,
							   std::vector<double>& d_vec,
                               int iterationNumber);
void Recv(int& max_num);
void Recv(double& max_num);
std::vector<double> get_result(double a, double b, double r,
							   double min_error_bound, double (*func)(double));
bool AreVectorsEquals(std::vector<double> vec1, std::vector<double> vec2);

#endif  // MODULES_TASK_3_PALGUEV_I_STRONGIN_IMPLEMENTATION_H_
