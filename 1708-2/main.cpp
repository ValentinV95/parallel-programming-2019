// Copyright 2020 Strelzov Roman
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <mpi.h>
#include <cmath>
#include <functional>
#include <utility>
#include <vector>
#include "./monte_carlo_mult_integr.h"

#define abs_error 0.75
const double PI = acos(-1);


double f1(std::vector<double> v) {
    double res = 1;
    for (int i = 0; i < v.size(); ++i)
        res *= v[i];
    return res;
}

double f2(std::vector<double> v) {
    double res = 0;
    for (int i = 0; i < v.size(); ++i)
        res += v[i];
    return sin(res);
}

double f3(std::vector<double> v) {
    double res = 0;
    for (int i = 0; i < v.size(); i++)
        res += v[i]*v[i];
    return res;
}

TEST(monteCarloMultipleIntegraion, Sequentional_On_One_Dimensional) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        std::vector<std::pair<double, double>> lims = {{0, 5}};
        double ans = monteCarloIntegraionSequentional(lims, 10000, f2);
        EXPECT_NEAR(ans, 0.716338, abs_error);
    }
}

TEST(monteCarloMultipleIntegraion, Sequentional_On_Two_Dimensional) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        std::vector<std::pair<double, double>> lims = {{0, 1}, {0, 1}};
        double ans = monteCarloIntegraionSequentional(lims, 10000, f3);
        EXPECT_NEAR(ans, 0.25, abs_error);
    }
}

TEST(monteCarloMultipleIntegraion, Sequentional_On_Three_Dimensional) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        std::vector<std::pair<double, double>> lims = {{0, 1}, {0, 1}, {0, 1}};
        double ans = monteCarloIntegraionSequentional(lims, 10000, f3);
        EXPECT_NEAR(ans, 1, abs_error);
    }
}

TEST(monteCarloMultipleIntegraion, Parallel_On_One_Dimensional) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<std::pair<double, double>> lims = generate_limits(1, 0, 1);
    double ans = monteCarloIntegraionParallel(lims, 10000, f1);
    if (rank == 0) {
        double ans_check = monteCarloIntegraionSequentional(lims, 10000, f1);
        EXPECT_NEAR(ans, ans_check, abs_error);
    }
}

TEST(monteCarloMultipleIntegraion, Parallel_On_Two_Dimensional) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<std::pair<double, double>> lims = generate_limits(2, 0, 1);
    double ans = monteCarloIntegraionParallel(lims, 10000, f2);
    if (rank == 0) {
        double ans_check = monteCarloIntegraionSequentional(lims, 10000, f2);
        EXPECT_NEAR(ans, ans_check, abs_error);
    }
}

TEST(monteCarloMultipleIntegraion, Parallel_On_Three_Dimensional) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<std::pair<double, double>> lims = generate_limits(3, 0, 1);
    double ans = monteCarloIntegraionParallel(lims, 10000, f3);
    if (rank == 0) {
        double ans_check = monteCarloIntegraionSequentional(lims, 10000, f3);
        EXPECT_NEAR(ans, ans_check, abs_error);
    }
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    MPI_Init(&argc, &argv);

    ::testing::AddGlobalTestEnvironment(new GTestMPIListener::MPIEnvironment);
    ::testing::TestEventListeners& listeners =
        ::testing::UnitTest::GetInstance()->listeners();

    listeners.Release(listeners.default_result_printer());
    listeners.Release(listeners.default_xml_generator());

    listeners.Append(new GTestMPIListener::MPIMinimalistPrinter);
    return RUN_ALL_TESTS();
}
