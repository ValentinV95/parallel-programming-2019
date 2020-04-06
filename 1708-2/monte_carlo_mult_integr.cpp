// Copyright 2020 Strelzov Roman
#include "../../../modules/task_3/strelzov_r_monte_carlo_mult_integr/monte_carlo_mult_integr.h"
#include <mpi.h>
#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>
#include <vector>
#include <utility>

double monteCarloIntegraionParallel(const std::vector<std::pair<double, double>> lims, int dots_cnt,
                                    double(*func)(const std::vector<double>)) {
    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int lims_size = lims.size();

    std::mt19937 mt;
    std::vector<int>mtrank_seed(size);
    if (rank == 0) {
        mt.seed(time(0));
        std::uniform_int_distribution<int> r(INT_MIN, INT_MAX);
        for (int i = 0; i < size; i++)
            mtrank_seed[i] = r(mt);
    }
    int seed;
    MPI_Scatter(mtrank_seed.data(), 1, MPI_INT, &seed, 1, MPI_INT, 0, MPI_COMM_WORLD);
    mt.seed(seed);

    int dim_cnt = lims_size;
    std::vector<std::uniform_real_distribution<double>> r(dim_cnt);
    for (int i = 0; i < dim_cnt; i++) {
        r[i] = std::uniform_real_distribution<double>(lims[i].first, lims[i].second);
    }
    double local = 0, ans = 0;

    double dots_cnt_proc = dots_cnt / size;
    if (dots_cnt%size >= rank)
        ++dots_cnt;

    for (int i = 0; i < dots_cnt_proc; ++i) {
        std::vector<double> dot;
        for (int j = 0; j < dim_cnt; ++j)
            dot.push_back(r[j](mt));
        local += func(dot);
    }

    MPI_Reduce(&local, &ans, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (!rank) {
        for (int i = 0; i < dim_cnt; i++) {
            ans *= (lims[i].second - lims[i].first);
        }
        ans /= dots_cnt;
    }
    return ans;
}

double monteCarloIntegraionSequentional(const std::vector<std::pair<double, double>> lims, int dots_cnt,
                                            double(*func)(const std::vector<double>)) {
    int lims_size = lims.size();
    int dim_cnt = lims_size;
    double res = 0;
    std::mt19937 mt;
    mt.seed(time(0));
    std::vector<std::uniform_real_distribution<double>> r(dim_cnt);
    for (int i = 0; i < dim_cnt; i++) {
        r[i] = std::uniform_real_distribution<double>(lims[i].first, lims[i].second);
    }
    for (int i = 0; i < dots_cnt; ++i) {
        std::vector<double> dot;
        for (int j = 0; j < dim_cnt; ++j)
            dot.push_back(r[j](mt));
        res += func(dot);
    }
    for (int i = 0; i < dim_cnt; i++) {
        res *= (lims[i].second - lims[i].first);
    }
    res /= dots_cnt;
    return res;
}


std::vector<std::pair<double, double>> generate_limits(int size, double lower_bound, double upper_bound) {
    std::mt19937 mt;
    mt.seed(time(0));
    std::uniform_real_distribution<double> rand(lower_bound, upper_bound);
    std::vector<std::pair<double, double>> res(size);
    for (int i = 0; i < size; i++) {
        double t1 = rand(mt), t2 = rand(mt);
        res[i].first = std::min(t1, t2);
        res[i].second = std::max(t1, t2);
    }
    return res;
}
