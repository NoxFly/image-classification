#pragma once

#include <filesystem>
#include <chrono>
#include <limits>
#include <thread>
#include <stack>
#include <mutex>
#include <condition_variable>

#include "file.hpp"
#include "common.hpp"
#include "image.hpp"
#include "distances.hpp"


struct TestResult {
	size_t fileCount;
	uint successCount;
	float accuracy;
	float loss;
};


TestResult test(const bool multithreaded, const std::string& dataset, VecHistos& histosTrainFree, VecHistos& histosTrainFull);

TestResult test_seq(const std::string& path, PlaceEnum expectation, VecHistos& modelsFree, VecHistos& modelsFull);
TestResult test_par(const std::string& path, PlaceEnum expectation, VecHistos& modelsFree, VecHistos& modelsFull);
