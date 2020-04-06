#include <gtest-mpi-listener.hpp>
#include <vector>
#include "implementation.h"
#include "transform_functions.h"
#include <gtest/gtest.h>
#include <cmath>

bool AreVectorsEquals(std::vector<double> vec1, std::vector<double> vec2)
{
	for (int i = 0; i < static_cast<int>(vec1.size()); i++)
	{
		if (!(std::fabs(vec1[i] - vec2[i]) < 1e-2)) return false;
	}
	return true;
}

TEST(Global_Search_MPI, Fail2)
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	std::vector<double> actual(2);

	if (rank == 0) {
		ASSERT_ANY_THROW(actual = functions::Impl::get_result(5.0, 5.0, 2.0, 0.0001, TransformFunction1));
	}
}

TEST(Global_Search_MPI, FailWrongLength)
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	std::vector<double> actual(2);

	if (rank == 0) {
		EXPECT_EQ(0, rank);
		ASSERT_ANY_THROW(actual = functions::Impl::get_result(5.0, 2.0, 1.0, 0.00001, TransformFunction2));
	}
}

TEST(Global_Search_MPI, TransformFunction1)
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	const auto vec = functions::Impl::get_result(1.0, 10.0, 2.0, 0.00001, TransformFunction1);

	if (rank == 0) {
		const std::vector<double> expected{ 99.161, 1.00001 };
		EXPECT_EQ(1, AreVectorsEquals(vec, expected));
	}
}

TEST(Global_Search_MPI, TransformFunction2)
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	const auto vec = functions::Impl::get_result(1.0, 2.0, 2.0, 0.0001, TransformFunction2);

	if (rank == 0) {
		const std::vector<double> expected{ 0.841, 1.000 };
		EXPECT_EQ(1, AreVectorsEquals(vec, expected));
	}
}

TEST(Global_Search_MPI, TransformFunction3)
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	const auto vec = functions::Impl::get_result(1.0, 50.0, 2.0, 0.00001, TransformFunction3);

	if (rank == 0) {
		const std::vector<double> expected{ 0.530546, 1.00001 };
		EXPECT_EQ(1, AreVectorsEquals(vec, expected));
	}
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	MPI_Init(&argc, &argv);

	AddGlobalTestEnvironment(new GTestMPIListener::MPIEnvironment);
	testing::TestEventListeners& listeners =
		testing::UnitTest::GetInstance()->listeners();

	listeners.Release(listeners.default_result_printer());
	listeners.Release(listeners.default_xml_generator());

	listeners.Append(new GTestMPIListener::MPIMinimalistPrinter);
	return RUN_ALL_TESTS();
}
