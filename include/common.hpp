#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <map>


// Histograms
#define HISTO_PIXEL_COUNT 256
#define IMAGE_CARVING_COUNT 14
#define SAMPLE_COUNT 9
#define HISTO_TOTAL_SIZE HISTO_PIXEL_COUNT * IMAGE_CARVING_COUNT * SAMPLE_COUNT

// Paths
#define RESOURCES_PATH "res/"
#define TEST_DIR "TEST/"
#define TRAIN_DIR "TRAIN/"

// Namespaces
using namespace cv;
using namespace std::chrono;

// Typedefs
typedef time_point<std::chrono::system_clock> tp;

typedef unsigned int uint;
typedef unsigned short ushort;

typedef float* ImageHisto; // must be of size HISTO_TOTAL_SIZE when allocated
typedef std::vector<ImageHisto> VecHistos; // its size will be the number of images in the training set
typedef std::map<std::string, VecHistos> MapHistos;
