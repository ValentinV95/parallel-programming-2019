#include <gtest/gtest.h>
#include <gtest-mpi-listener.hpp>
#include <vector>
#include "./implementation.cpp"
#include "./transform_functions.h"

TEST(Global_Search_MPI, Test_Fail_2)
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	std::vector<double> actual(2);

	EXPECT_EQ(0, rank);
	ASSERT_ANY_THROW(actual = get_result(5.0, 5.0, 2.0, 0.0001, TransformFunction1));
}

TEST(Global_Search_MPI, Test_Fail_Wrong_Length)
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	std::vector<double> actual(2);

	EXPECT_EQ(0, rank);
	ASSERT_ANY_THROW(actual = get_result(5.0, 2.0, 1.0, 0.00001, TransformFunction2));
}

TEST(Global_Search_MPI, Test_Transform_Function1)
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	const auto vec = get_result(1.0, 10.0, 2.0, 0.00001, TransformFunction1);

	EXPECT_EQ(0, rank);
	const std::vector<double> expected{99.161, 1.00001};
	EXPECT_EQ(1, AreVectorsEquals(vec, expected));
}

TEST(Global_Search_MPI, Test_Transform_Function2)
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	const auto vec = get_result(1.0, 2.0, 2.0, 0.0001, TransformFunction2);

	EXPECT_EQ(0, rank);
	const std::vector<double> expected{0.841, 1.000};
	EXPECT_EQ(1, AreVectorsEquals(vec, expected));
}

TEST(Global_Search_MPI, Test_Transform_Function3)
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	const auto vec = get_result(1.0, 50.0, 2.0, 0.00001, TransformFunction3);

	EXPECT_EQ(0, rank);
	const std::vector<double> expected{0.530546, 1.00001};
	EXPECT_EQ(1, AreVectorsEquals(vec, expected));
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	MPI_Init(&argc, &argv);

	AddGlobalTestEnvironment(new GTestMPIListener::MPIEnvironment);
	testing::TestEventListeners& listeners =
		testing::UnitTest::GetInstance()->listeners();

	listeners.Release(listeners.default_result_printer());
	listeners.Release(listeners.default_xml_generator());

	listeners.Append(new GTestMPIListener::MPIMinimalistPrinter);
	return RUN_ALL_TESTS();
}
