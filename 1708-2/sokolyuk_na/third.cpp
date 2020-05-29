#include <tbb/tbb.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <vector>
#include <queue>
#include <limits>
#include <cmath>
#include <algorithm>
#include <utility>
#include <time.h>
#include <iostream>
#include <random>

#define MIN_THREADS 4

std::vector<int> getRandomVec(int size) {
	if (size < 0) {
		throw "Error: -Size";
	}
	std::mt19937 gen;
	gen.seed(static_cast<unsigned int>(time(0)));
	std::vector<int> new_way(size);
	for (int i = 0; i < size; i++) {
		new_way[i] = static_cast<unsigned int>(gen() % 6);
		std::cout <<"\n\t"<< new_way[i] <<"\n"; std::fflush;;
	}
	return new_way;
}

std::vector<int> HandMadeVec() {
	std::vector<int> new_way;
		new_way = { 0, 7, 9, 0, 0, 14,
						  7, 0, 10, 15, 0, 0,
						  9, 10, 0, 11, 0, 2,
						  0, 15, 11, 0, 6, 0,
						  0, 0, 0, 6, 0, 9,
						  14, 0, 2, 0, 9, 0 };
		std::cout << "\n\tEnter vec: " << std::endl; std::fflush;;
		for (int i = 0; i < new_way.size(); i++) {
			std::cout << new_way[i]; std::fflush;;
		}
	return new_way;
}

std::vector<int> HandEnterVec(int size) {
	if (size < 0) {
		throw "Error: -Size";
	}
	std::vector<int> new_way(size);
	for (int i = 0; i < size; i++) {
		std::cout << "\n["<<i<<"]: "; std::fflush;;
		std::cin >> new_way[i];
	}
	return new_way;
}

std::vector<int> searchMinWay(std::vector<int> graph, int start, int end) {
	if (start == end)
		return std::vector<int>(1, 0);

	if (start > end) {
		std::swap(start, end);
	}

	int points_count = static_cast<int>(sqrt(graph.size()));
	int max_weight = INT_MAX;
	int min, min_point, temp;
	tbb::mutex mutex;
	tbb::task_scheduler_init(MIN_THREADS);
	std::vector<int> points_len(points_count, max_weight);
	std::vector<int> way;
	std::vector<bool> visisted(points_count, false);
	std::vector<int> min_vals(2, INT_MAX);

	--start;
	--end;

	points_len[start] = 0;

	do {
		min_vals = tbb::parallel_reduce(
			tbb::blocked_range<int>(0, points_count),
			std::vector<int>(2) = { INT_MAX, INT_MAX },
			[&](const tbb::blocked_range<int>& v, std::vector<int> local_min_vals) {
			for (int i = v.begin(); i < v.end(); i++) {
				if (!visisted[i] && points_len[i] < local_min_vals[0]) {
					local_min_vals[0] = points_len[i];
					local_min_vals[1] = i;
				}
			}
			return local_min_vals;
		},
			[&](std::vector<int> x, std::vector<int> y) {
			if (x[0] < y[0]) {
				return x;
			}
			return y;
		});

		min_point = min_vals[1];
		min = min_vals[0];

		if (min_point != max_weight) {
			tbb::parallel_for(
				tbb::blocked_range<int>(0, points_count),
				[&](const tbb::blocked_range<int>& v) {
				for (int i = v.begin(); i < v.end(); i++) {
					if (graph[min_point * points_count + i] > 0) {
						mutex.lock();
						temp = min + graph[min_point * points_count + i];
						if (points_len[i] > temp) {
							points_len[i] = temp;
						}
						mutex.unlock();
					}
				}
			});
			visisted[min_point] = true;
		}
	} while (min_point < max_weight);

	way.push_back(end + 1);
	int weight = points_len[end];

	while (end != start) {
		tbb::parallel_for(
			tbb::blocked_range<int>(0, points_count),
			[&](const tbb::blocked_range<int>& v) {
			for (int i = v.begin(); i < v.end(); i++) {
				if (graph[end * points_count + i] > 0) {
					temp = weight - graph[end * points_count + i];
					if (points_len[i] == temp) {
						weight = temp;
						end = i;
						mutex.lock();
						way.push_back(i + 1);
						mutex.unlock();
					}
				}
			}
		});
	}

	return way;
}

int main(int argc, char* argv[]) {
	int size, start=1, end=5;
	std::vector<int> way;
	std::cout << "\n\n\t\tSearch for shortest paths from one vertex (Dijkstra's algorithm).realization TBB programm. Sokolyk.N.. 381708-2" << std::endl;
	std::cout << "\n\tExample: " << std::endl; std::fflush;;
	way = HandMadeVec();
	way = searchMinWay(way, start, end);
	std::cout << "\n\tResult ver: " << std::endl; std::fflush;;
	for (int i = 0; i < way.size(); i++) {
		std::cout << way[i]; std::fflush;;
	}
	std::cout << "\n\tSize: " << std::endl; std::fflush;;
	std::cin >> size;
	std::cout << "\n\tStart: " << std::endl; std::fflush;;
	std::cin >> start;
	std::cout << "\n\tEnd: " << std::endl; std::fflush;;
	std::cin >> end;
	way = HandEnterVec(size);
	way = searchMinWay(way, start, end);
	for (int i = 0; i < size; i++) {
		std::cout << way[i]; std::fflush;;
	}
	system("pause");
}