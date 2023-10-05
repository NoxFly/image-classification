#pragma once

#include <common.hpp>

double L1Distance(const ImageHisto& data, const ImageHisto& model, const size_t size, const size_t sizeMod4);
double CosineDistance(const ImageHisto& data, const ImageHisto& model);