#pragma once

#include <filesystem>
#include <chrono>
#include <limits>
#include <thread>
#include <stack>
#include <mutex>
#include <condition_variable>
#include <algorithm>

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


void test(MapHistos* models);

