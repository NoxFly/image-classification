#pragma once

#include <common.hpp>
#include <cmath>

float L1Distance(ImageHisto data, ImageHisto model, const size_t size);
float BhattacharyyaDistance(ImageHisto data, ImageHisto model, const size_t size);