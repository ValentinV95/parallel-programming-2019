// Copyright 2020 Strelzov Roman
#ifndef MODULES_TASK_3_STRELZOV_R_MONTE_CARLO_MULT_INTEGR_MONTE_CARLO_MULT_INTEGR_H_
#define MODULES_TASK_3_STRELZOV_R_MONTE_CARLO_MULT_INTEGR_MONTE_CARLO_MULT_INTEGR_H_
#include <vector>
#include <utility>

double monteCarloIntegraionParallel(const std::vector<std::pair<double, double>> lims, int dots_cnt,
                                    double(*func)(const std::vector<double>));
double monteCarloIntegraionSequentional(const std::vector<std::pair<double, double>> lims, int dots_cnt,
                                            double(*func)(const std::vector<double>));
std::vector<std::pair<double, double>> generate_limits(int size, double lower_bound, double upper_bound);
#endif  // MODULES_TASK_3_STRELZOV_R_MONTE_CARLO_MULT_INTEGR_MONTE_CARLO_MULT_INTEGR_H_
