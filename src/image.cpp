#include "image.hpp"

#include <fstream>

Mat1b splitRGBChannels(const Mat3b& src) {
	const uint width = src.cols;
	const uint height = src.rows;

	Mat1b dest = Mat(height, 3 * width, CV_8UC1, Scalar(0, 0, 0));

	for (uint y = 0; y < height; y++) {
		for (uint x = 0; x < width; x++) {
			const Vec3b px = src(y, x);

			dest(y, x) = px[0];
			dest(y, x + width) = px[1];
			dest(y, x + width * 2) = px[2];
		}
	}

	return dest;
}

void edges(const Mat1b& src, Mat1b& dest, const int w) {
	Mat kernel = (Mat_<int>(3, 3) << -1, -1, -1, -1, w, -1, -1, -1, -1);
	filter2D(src, dest, -1, kernel);
}

void histogram(
	const Mat1b src,
	ImageHisto& arr,
	const uint startIndex = 0,
	const uint sx = 0,
	const uint sy = 0,
	const uint sw = 0,
	const uint sh = 0
) {
	const uint fsw = (sw == 0) ? src.cols : sw;
	const uint fsh = (sh == 0) ? src.rows : sh;

	const auto histoTotalSize = HISTO_TOTAL_SIZE;
	const auto histoTotalOffset = HISTO_TOTAL_SIZE - HISTO_PIXEL_COUNT;

	//std::cout << startIndex << " " << histoTotalSize << " " << histoTotalOffset << std::endl;

	if (startIndex > HISTO_TOTAL_SIZE - HISTO_PIXEL_COUNT) {
		throw std::runtime_error("Index out of bounds for startIndex (" + std::to_string(startIndex) + ")");
	}

	if (sx + fsw > (uint)src.cols || sy + fsh > (uint)src.rows) {
		throw std::runtime_error(
			"Index out of bounds for sx, sy, sw or sh ("
			+ std::to_string(sx) + ", "
			+ std::to_string(sy) + ", "
			+ std::to_string(sw) + ", "
			+ std::to_string(sh) +
			")"
		);
	}

	const uint dataLength = fsw * fsh;
	const uint histoMax = startIndex + HISTO_PIXEL_COUNT;
	const uint xmax = sx + fsw;
	const uint ymax = sy + fsh;

	for (uint i = startIndex; i < histoMax; i++) {
		arr[i] = 0.f;
	}

	for (uint y = sy; y < ymax; y++) {
		for (uint x = sx; x < xmax; x++) {
			const uint value = (uint)src(y, x);
			arr[startIndex + value]++;
		}
	}

	for (uint i = startIndex; i < histoMax; i++) {
		arr[i] /= dataLength;
	}
}

ImageHisto emptyHisto() {
	ImageHisto hist = (ImageHisto)malloc(sizeof(float) * HISTO_TOTAL_SIZE);
	
	for (uint i = 0; i < HISTO_TOTAL_SIZE; i++) {
		hist[i] = 0;
	}

	return hist;
}


/* --------------------------------------------------- */



void processImage(const std::string& path, ImageHisto hist) {
	Mat3b img = imread(path, IMREAD_COLOR);

	if (img.empty()) {
		throw std::runtime_error("Failed to open image " + path);
	}

	auto img3Chan = splitRGBChannels(img);

	Mat1b imgGray,		// plastic
		imgGrayEdge0,	// cardboard
		img3ChanLBP,	// glass + metal + plastic + trash
		img3Chan3Edge9,	// paper
		img3ChanEdge9;	// trash

	cvtColor(img, imgGray, COLOR_BGR2GRAY);


	LBP(img3Chan, img3ChanLBP);
	edges(img3Chan, img3ChanEdge9, 9);
	edges(img3Chan, img3Chan3Edge9, 9);
	edges(imgGray, imgGrayEdge0, 0);

	const uint sizes[][3] = {
		{ 1, (uint)img.cols, (uint)img.rows },
		{ 2, (uint)img.cols / 2, (uint)img.rows / 2 },
		{ 3, (uint)img.cols / 3, (uint)img.rows / 3 },
	};

	const uint sizesCount = 3;

	uint l = 0;
	uint ll = 0;

	for (uint i = 0; i < sizesCount; i++) {
		// images with original size
		for (uint j = 0; j < sizes[i][0]; j++) {
			for (uint k = 0; k < sizes[i][0]; k++) {
				const uint sx = j * sizes[i][1];
				const uint sy = k * sizes[i][2];

				const uint sw = sizes[i][1];
				const uint sh = sizes[i][2];

				histogram(imgGrayEdge0, hist, l * HISTO_PIXEL_COUNT, sx, sy, sw, sh);
				histogram(img3ChanLBP, hist, (IMAGE_CARVING_COUNT + l) * HISTO_PIXEL_COUNT, sx, sy, sw, sh);
				histogram(img3ChanEdge9, hist, (2 * IMAGE_CARVING_COUNT + l) * HISTO_PIXEL_COUNT, sx, sy, sw, sh);

				l++;
			}
		}

		const uint sw = sizes[i][1] / 3;
		const uint sh = sizes[i][2];
		const uint startIdx = 3 * IMAGE_CARVING_COUNT;

		// images with 3 * width size
		for (uint v = 0; v < 3; v++) {
			for (uint j = 0; j < sizes[i][0]; j++) {
				for (uint k = 0; k < sizes[i][0]; k++) {
					const uint sx = v * img.cols + j * sizes[i][1];
					const uint sy = k * sizes[i][2];

					histogram(img3Chan3Edge9, hist, (startIdx + ll) * HISTO_PIXEL_COUNT, sx, sy, sw, sh);

					ll++;
				}
			}
		}
	}

}