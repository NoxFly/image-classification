#include "testing.hpp"

using namespace std;
namespace fs = filesystem;


// -------------------------------------------------------------------------------------
// --------------------------------------- TESTS ---------------------------------------
// -------------------------------------------------------------------------------------

void logTestResults(const TestResult& results, const PlaceEnum& type, const duration<double>& timeElapsed) {
	std::stringstream accStream;
	accStream << std::fixed << std::setprecision(2) << results.accuracy;

	std::stringstream lossStream;
	lossStream << std::fixed << std::setprecision(2) << results.loss;

	const string fileCount = to_string(results.fileCount);
	const string accuracy = accStream.str();
	const string loss = lossStream.str();

	const string spacesFileCount = string(11 - fileCount.length(), ' ');
	const string spacesAccuracy = string(8 - accuracy.length(), ' ');
	const string spacesLoss = string(5 - loss.length(), ' ');

	cout
		<< (type == PlaceEnum::FREE ? "FREE" : "FULL") << " | "
		<< fileCount << spacesFileCount << "| "
		<< accuracy << "%" << spacesAccuracy << "| "
		<< loss << "%" << spacesLoss << "| "
		<< (timeElapsed.count() * 1000) << " ms"
		<< endl;
}

TestResult test(const bool multithreaded, const string& dataset, VecHistos& histosTrainFree, VecHistos& histosTrainFull) {
	cout
		<< "\nTYPE | File count | accuracy | loss  | duration"
		<< "\n-----|------------|----------|-------|---------"
		<< endl;

	const string testPath = RESOURCES_PATH "Dataset" + dataset + "/" TEST_DIR;

	const auto testFn = multithreaded ? test_par : test_seq;

	tp t0 = system_clock::now();
	const TestResult freeResults = testFn(testPath + "Free", PlaceEnum::FREE, histosTrainFree, histosTrainFull);
	tp t1 = system_clock::now();

	duration<double> testingFreeTime = t1 - t0;

	logTestResults(freeResults, PlaceEnum::FREE, testingFreeTime);

	tp t2 = system_clock::now();
	const TestResult fullResults = testFn(testPath + "Full", PlaceEnum::FULL, histosTrainFree, histosTrainFull);
	tp t3 = system_clock::now();

	const duration<double> testingFullTime = t3 - t2;
	const duration<double> testingTotalTime = testingFullTime + testingFreeTime;

	logTestResults(fullResults, PlaceEnum::FULL, testingFullTime);

	const float acc = (freeResults.accuracy + fullResults.accuracy) / 2;

	TestResult results = {
		freeResults.fileCount + fullResults.fileCount,
		freeResults.successCount + fullResults.successCount,
		acc,
		100 - acc
	};

	cout << "\nTesting duration : " << (testingTotalTime.count() * 1000) << " ms\n" << endl;

	return results;
}




/**
 * Atomic function used by both sequential and parallel functions to process the test on an image.
 *
 * @param path - The image's path
 * @param modelsFree - The matrix of histograms that contains all resulted histograms from training images for free places.
 * @param modelsFull - The matrix of histograms that contains all resulted histograms from training images for taken places.
 */
PlaceEnum testFile(const string& path, const VecHistos& modelsFree, const VecHistos& modelsFull) {
	// create histogram for the test image
	ImageHisto hist;

	processImage(path, hist);

	const size_t size = hist.size();
	const size_t sizeMod4 = size - (size % 4);

	double freeMin = numeric_limits<double>::max();

	for (size_t i = 0; i < modelsFull.size(); i++) {
		const double localSum = L1Distance(hist, modelsFree[i], size, sizeMod4);

		if (localSum < freeMin) {
			freeMin = localSum;
		}
	}

	// test test image with all train.full
	for (size_t i = 0; i < modelsFull.size(); i++) {
		const double localSum = L1Distance(hist, modelsFull[i], size, sizeMod4);

		// if the sum of distances of this train.full histogram is lower than the minimum sum of distances
		// of the train.free models, then it's a Full
		if (localSum < freeMin) {
			return PlaceEnum::FULL;
		}
	}

	return PlaceEnum::FREE;
}


/**
 * Testing function SEQUENTIAL execution
 * Detects all available cores of the CPU
 * Separates image processing over all.
 *
 * @param path - The directory path that contains all images to be proceed.
 * @param expectation - The expected result (free or full) of image's analysis.
 * @param modelsFree - The matrix of histograms that contains all resulted histograms from training images for free places.
 * @param modelsFull - The matrix of histograms that contains all resulted histograms from training images for taken places.
 */
TestResult test_seq(const string& path, PlaceEnum expectation, VecHistos& modelsFree, VecHistos& modelsFull) {
	const size_t fileCount = fileCountInDir(path);

	uint success = 0;

	for (const auto& entry : fs::directory_iterator(path)) {
		string filepath = entry.path().string();

		if (testFile(filepath, modelsFree, modelsFull) == expectation) {
			success++;
		}
	}

	const float acc = ((float)(success * 100) / fileCount);

	return {
		fileCount,
		success,
		acc,
		100 - acc
	};
}


void processTestFilePar(
	stack<string>& fileStack,
	VecHistos& modelsFree, VecHistos& modelsFull,
	uint& successCounter,
	PlaceEnum& expectation,
	mutex& mtx, condition_variable& cv
) {
	while (true) {
		string currentFile;

#ifdef _DEBUG
		int idx;
#endif

		{
			unique_lock<mutex> lock(mtx);

			if (fileStack.empty()) {
				break;
			}

			currentFile = fileStack.top();
			fileStack.pop();

#ifdef _DEBUG
			idx = fileStack.size();
			cout << "(" << idx << ")" << "processImage : " << currentFile << endl;
#endif
		}

		if (testFile(currentFile, modelsFree, modelsFull) == expectation) {
			{
				unique_lock<mutex> lock(mtx);
#ifdef _DEBUG
				cout << "  (" << idx << ") Test succeed" << endl;
#endif
				successCounter++;
			}
		}
#ifdef _DEBUG
		else {
			{
				unique_lock<mutex> lock(mtx);
				cout << "  (" << idx << ") Test failed" << endl;
			}
		}
#endif

		cv.notify_one();
	}
}

/**
 * Testing function PARALLEL execution
 * Detects all available cores of the CPU
 * Separates image processing over all.
 *
 * @param path - The directory path that contains all images to be proceed.
 * @param expectation - The expected result (free or full) of image's analysis.
 * @param modelsFree - The matrix of histograms that contains all resulted histograms from training images for free places.
 * @param modelsFull - The matrix of histograms that contains all resulted histograms from training images for taken places.
 */
TestResult test_par(const string& path, PlaceEnum expectation, VecHistos& modelsFree, VecHistos& modelsFull) {
	const size_t fileCount = fileCountInDir(path);

	uint success = 0;

	const uint coreCount = thread::hardware_concurrency();

	// build stack
	stack<string> fileStack;

	for (const auto& entry : fs::directory_iterator(path)) {
		fileStack.push(entry.path().string());
	}

	vector<thread> threads;

	// create thread polling
	mutex mtx;
	condition_variable cv;

#ifdef _DEBUG
	cout << endl;
#endif

	for (uint i = 0; i < coreCount; ++i) {
		threads.emplace_back(
			processTestFilePar,
			ref(fileStack),
			ref(modelsFree), ref(modelsFull),
			ref(success),
			ref(expectation),
			ref(mtx), ref(cv)
		);
	}

	for (thread& t : threads) {
		t.join();
	}

	const float acc = ((float)(success * 100) / fileCount);
	

	return {
		fileCount,
		success,
		acc,
		100 - acc
	};
}