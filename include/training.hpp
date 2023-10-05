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


void train(MapHistos* histos);