#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <array>


// Histograms
#define HISTO_PIXEL_COUNT 256
#define IMAGE_CARVING_COUNT 14
#define HISTO_TOTAL_SIZE HISTO_PIXEL_COUNT * IMAGE_CARVING_COUNT

// Paths
#define RESOURCES_PATH "res/"
#define TEST_DIR "Test/"
#define TRAIN_DIR "Train/"
#define FREE_DIR "Free/"
#define FULL_DIR "Full/"

// Namespaces
using namespace cv;
using namespace std::chrono;

// Typedefs
typedef time_point<std::chrono::system_clock> tp;

typedef unsigned int uint;
typedef unsigned short ushort;

typedef std::array<float, HISTO_TOTAL_SIZE> ImageHisto;
typedef std::vector<ImageHisto> VecHistos;

// Enums
enum class PlaceEnum {
	FREE,
	FULL
};