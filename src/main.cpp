#include "common.hpp"
#include "training.hpp"
#include "testing.hpp"
#include "image.hpp"

#include <fstream>

int main(int, char**) {
	MapHistos* histos = new MapHistos();

	train(histos);
	test(histos);

	delete histos;

	return EXIT_SUCCESS;
}