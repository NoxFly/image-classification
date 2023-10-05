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


void train(const bool multithreaded, const std::string& dataset, VecHistos& histosTrainFree, VecHistos& histosTrainFull);

void train_seq(const std::string& path, VecHistos& histos);
void train_par(const std::string& path, VecHistos& histos);
