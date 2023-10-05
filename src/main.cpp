#include <filesystem>
#include <chrono>
#include <limits>
#include <thread>

#include "common.hpp"
#include "training.hpp"
#include "testing.hpp"


int main(int, char**) {
	char ans;

	std::cout << "Choose a dataset (1: 100, 2: 1000, 3: 10000)\n";

	do {
		std::cout << "> ";
		ans = std::cin.get();
		std::cin.ignore();
	} while (ans != '1' && ans != '2' && ans != '3');

	std::string dataset;

	switch (ans) {
		case '1':
			dataset = "100";
			break;
		case '2':
			dataset = "2k";
			break;
		case '3':
			dataset = "20k";
			break;
	}

	std::cout << std::endl;

	std::cout << "Enable multithreading (y/N) : ";
	ans = std::cin.get();

	if (ans != '\n') {
		std::cin.ignore();
	}

	const bool useMultithreading = ans == 'Y' || ans == 'y';

	if (useMultithreading) {
		std::cout << "Multithreading enabled.\nNumber of cores : " << std::thread::hardware_concurrency() << std::endl;
	}
	else {
		std::cout << "Running sequential version." << std::endl;
	}

	std::cout << std::endl;

	VecHistos histosTrainFree, histosTrainFull;

	train(useMultithreading, dataset, histosTrainFree, histosTrainFull);

	std::cout << "Launch tests (Y/n) : ";
	ans = std::cin.get();

	if (ans != '\n') {
		std::cin.ignore();
	}

	if (ans == 'N' || ans == 'n') {
		return EXIT_SUCCESS;
	}

	const TestResult results = test(useMultithreading, dataset, histosTrainFree, histosTrainFull);

	std::cout
		<< "Accuracy : " << results.accuracy << "%\n"
		<< "Loss : " << results.loss<< "%"
		<< std::endl;

	return EXIT_SUCCESS;
}