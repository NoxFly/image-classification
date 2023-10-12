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

float BhattacharyyaDistance(ImageHisto data, ImageHisto model, const size_t size) {

    float distance = 0.0;
    float bcFactor = 0.0;

    for (size_t i = 0; i < size; i++) {
        bcFactor += sqrt(data[i] * model[i]);
        distance += (data[i] + model[i]) / 2;
    }

    distance = -log(bcFactor) * distance;
    return distance;
}