#include "image.hpp"


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

void emptyHisto(ImageHisto& hist) {
	for (uint i = 0; i < hist.size(); i++) {
		hist[i] = 0;
	}
}


/* --------------------------------------------------- */



void processImage(const std::string& path, ImageHisto& hist) {
	Mat3b img = imread(path, IMREAD_COLOR);

	if (img.empty()) {
		throw std::runtime_error("Failed to open image " + path);
	}

	//auto img3Chan = splitRGBChannels(img);

	Mat1b imgGray, imgEdged;

	cvtColor(img, imgGray, COLOR_BGR2GRAY);

	//edges(imgGray, imgEdged, 10);
	LBP(imgGray, imgEdged);

	const uint sw3x3 = imgEdged.cols / 3;
	const uint sh3x3 = imgEdged.rows / 3;

	const uint sw2x2 = imgEdged.cols / 2;
	const uint sh2x2 = imgEdged.rows / 2;

	const uint sw1x1 = imgEdged.cols;
	const uint sh1x1 = imgEdged.rows;

	histogram(imgEdged, hist,  0 * HISTO_PIXEL_COUNT, 0, 0, sw3x3, sh3x3);					// 1	3x3		TL
	histogram(imgEdged, hist,  1 * HISTO_PIXEL_COUNT, sw3x3, 0, sw3x3, sh3x3);				// 2	3x3		TM
	histogram(imgEdged, hist,  2 * HISTO_PIXEL_COUNT, 2 * sw3x3, 0, sw3x3, sh3x3);			// 3	3x3		TR
	histogram(imgEdged, hist,  3 * HISTO_PIXEL_COUNT, 0, sh3x3, sw3x3, sh3x3);				// 4	3x3		ML
	histogram(imgEdged, hist,  4 * HISTO_PIXEL_COUNT, sw3x3, sh3x3, sw3x3, sh3x3);			// 5	3x3		MM
	histogram(imgEdged, hist,  5 * HISTO_PIXEL_COUNT, 2 * sw3x3, sh3x3, sw3x3, sh3x3);		// 6	3x3		MR
	histogram(imgEdged, hist,  6 * HISTO_PIXEL_COUNT, 0, 2 * sh3x3, sw3x3, sh3x3);			// 7	3x3		BL
	histogram(imgEdged, hist,  7 * HISTO_PIXEL_COUNT, sw3x3, 2 * sh3x3, sw3x3, sh3x3);		// 8	3x3		BM
	histogram(imgEdged, hist,  8 * HISTO_PIXEL_COUNT, 2 * sw3x3, 2 * sh3x3, sw3x3, sh3x3);	// 9	3x3		BR
	histogram(imgEdged, hist,  9 * HISTO_PIXEL_COUNT, 0, 0, sw2x2, sh2x2);					// 10	2x2		TL
	histogram(imgEdged, hist, 10 * HISTO_PIXEL_COUNT, sw2x2, 0, sw2x2, sh2x2);				// 11	2x2		TR
	histogram(imgEdged, hist, 11 * HISTO_PIXEL_COUNT, 0, sh2x2, sw2x2, sh2x2);				// 12	2x2		BL
	histogram(imgEdged, hist, 12 * HISTO_PIXEL_COUNT, sw2x2, sh2x2, sw2x2, sh2x2);			// 13	2x2		BR
	histogram(imgEdged, hist, 13 * HISTO_PIXEL_COUNT, 0, 0, sw1x1, sh1x1);					// 14	1x1		*
}


void processImageDebug() {
	Mat1b img = (Mat_<uchar>(3, 3) << 50, 123, 51, 126, 100, 52, 127, 53, 54);

	Mat1b imgLBP, imgEdged;

	std::cout << "Image width/height : " << img.cols << "/" << img.rows << std::endl;

	//edges(img, imgEdged, 10);
	LBP(img, imgLBP);

	std::cout << "----------------------------------- GRAY :" << std::endl;
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			std::cout << (int)img(i, j) << " ";
		}
		std::cout << "\n";
	}

	std::cout << "----------------------------------- LBP :" << std::endl;
	for (int i = 0; i < imgLBP.rows; i++) {
		for (int j = 0; j < imgLBP.cols; j++) {
			std::cout << (int)imgLBP(i, j) << " ";
		}
		std::cout << "\n";
	}

	/*std::cout << "----------------------------------- EDGES:" << std::endl;
	for (int i = 0; i < imgEdged.rows; i++) {
		for (int j = 0; j < imgEdged.cols; j++) {
			std::cout << (int)imgEdged(i, j) << " ";
		}
		std::cout << "\n";
	}*/

	std::cout << std::endl;

	const uint sw3x3 = imgEdged.cols / 3;
	const uint sh3x3 = imgEdged.rows / 3;

	const uint sw2x2 = imgEdged.cols / 2;
	const uint sh2x2 = imgEdged.rows / 2;

	const uint sw1x1 = imgEdged.cols;
	const uint sh1x1 = imgEdged.rows;

	ImageHisto hist;

	//histogram(imgEdged, hist, 0 * HISTO_PIXEL_COUNT, 0, 0, sw3x3, sh3x3);					// 1	3x3		TL
	//histogram(imgEdged, hist, 1 * HISTO_PIXEL_COUNT, sw3x3, 0, sw3x3, sh3x3);				// 2	3x3		TM
	//histogram(imgEdged, hist, 2 * HISTO_PIXEL_COUNT, 2 * sw3x3, 0, sw3x3, sh3x3);			// 3	3x3		TR
	//histogram(imgEdged, hist, 3 * HISTO_PIXEL_COUNT, 0, sh3x3, sw3x3, sh3x3);				// 4	3x3		ML
	//histogram(imgEdged, hist, 4 * HISTO_PIXEL_COUNT, sw3x3, sh3x3, sw3x3, sh3x3);			// 5	3x3		MM
	//histogram(imgEdged, hist, 5 * HISTO_PIXEL_COUNT, 2 * sw3x3, sh3x3, sw3x3, sh3x3);		// 6	3x3		MR
	//histogram(imgEdged, hist, 6 * HISTO_PIXEL_COUNT, 0, 2 * sh3x3, sw3x3, sh3x3);			// 7	3x3		BL
	//histogram(imgEdged, hist, 7 * HISTO_PIXEL_COUNT, sw3x3, 2 * sh3x3, sw3x3, sh3x3);		// 8	3x3		BM
	//histogram(imgEdged, hist, 8 * HISTO_PIXEL_COUNT, 2 * sw3x3, 2 * sh3x3, sw3x3, sh3x3);	// 9	3x3		BR
	//histogram(imgEdged, hist, 9 * HISTO_PIXEL_COUNT, 0, 0, sw2x2, sh2x2);					// 10	2x2		TL
	//histogram(imgEdged, hist, 10 * HISTO_PIXEL_COUNT, sw2x2, 0, sw2x2, sh2x2);				// 11	2x2		TR
	//histogram(imgEdged, hist, 11 * HISTO_PIXEL_COUNT, 0, sh2x2, sw2x2, sh2x2);				// 12	2x2		BL
	//histogram(imgEdged, hist, 12 * HISTO_PIXEL_COUNT, sw2x2, sh2x2, sw2x2, sh2x2);			// 13	2x2		BR
	histogram(imgLBP, hist, 0 * HISTO_PIXEL_COUNT, 0, 0, sw1x1, sh1x1);

	std::cout << "----------------------------------- HISTO :" << std::endl;

	for (uint i = 0; i < HISTO_PIXEL_COUNT; i++) {
		std::cout << hist[i] << " ";

		if (++i % 256 == 0) {
			std::cout << "\n";
		}
	}

	std::cout << std::endl;
}