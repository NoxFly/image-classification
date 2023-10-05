#pragma once

#include "common.hpp"
#include "LBP.hpp"

Mat1b splitRGBChannels(const Mat3b& src);

void processImage(const std::string& path, ImageHisto& hist);
void processImageDebug();

void emptyHisto(ImageHisto& hist);