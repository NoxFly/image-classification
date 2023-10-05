#include "distances.hpp"


/**
 * Also known as Manhattan distance.
 */
float L1Distance(ImageHisto data, ImageHisto model, const size_t size) {
	float distance = 0.0;

	for (size_t i = 0; i < size; i++) {
		distance += abs(data[i] - model[i]);
	}

	return distance;
}