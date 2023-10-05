#include "distances.hpp"

//
float fast_sqrt(const float x) {
	uint32_t i = *(uint32_t*)&x;
	i = 0x1fbd1df5 + (i >> 1);
	float y = *(float*)&i;
	return y;
}
//

/**
 * Also known as Manhattan distance.
 */
double L1Distance(const ImageHisto& data, const ImageHisto& model, const size_t size, const size_t sizeMod4) {
	double distance = 0.0f;

	const float* histData = data.data();
	const float* modelData = model.data();

	for (size_t j = 0; j < size; j += 4) {
		__m128 h = _mm_load_ps(histData + j);
		__m128 c = _mm_load_ps(modelData + j);
		__m128 diff = _mm_sub_ps(h, c);
		__m128 absDiff = _mm_andnot_ps(_mm_set1_ps(-0.0f), diff);
		__m128 sumDiff = _mm_hadd_ps(absDiff, absDiff);
		__m128 sum = _mm_hadd_ps(sumDiff, sumDiff);
		distance += _mm_cvtss_f32(sum);
	}

	for (size_t j = sizeMod4; j < size; j++) {
		distance += abs(histData[j] - model[j]);
	}

	return distance;
}

/**
 * On the paper, should be better than L1 distance for image processing.
 * However, after some runs, it appears that it takes more time than L1 distance, for lower accuracy.
 */
double CosineDistance(const ImageHisto& data, const ImageHisto& model) {
	double dot_product = 0.0, norm1 = 0.0, norm2 = 0.0;

	const float* histData = data.data();
	const float* modelData = model.data();

	const size_t histSize = data.size();

	for (size_t i = 0; i < histSize; ++i) {
		dot_product += histData[i] * modelData[i];
		norm1 += histData[i] * histData[i];
		norm2 += modelData[i] * modelData[i];
	}

	return dot_product / (fast_sqrt(norm1) * fast_sqrt(norm2));
}