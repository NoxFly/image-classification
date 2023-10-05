#include "testing.hpp"

#include <fstream>


using namespace std;
namespace fs = filesystem;


TestResult testWithModels_seq(const string& path, const string& expectation, MapHistos* models);


// -------------------------------------------------------------------------------------
// --------------------------------------- TESTS ---------------------------------------
// -------------------------------------------------------------------------------------

#ifndef _DEBUG
void logTestResults(const TestResult& results, const string& type, const duration<float>& timeElapsed) {
	std::stringstream accStream;
	accStream << std::fixed << std::setprecision(2) << results.accuracy;

	std::stringstream lossStream;
	lossStream << std::fixed << std::setprecision(2) << results.loss;

	const string fileCount = to_string(results.fileCount);
	const string accuracy = accStream.str();
	const string loss = lossStream.str();

	const string spacesType = string(12 - type.length(), ' ');
	const string spacesFileCount = string(11 - fileCount.length(), ' ');
	const string spacesAccuracy = string(8 - accuracy.length(), ' ');
	const string spacesLoss = string(6 - loss.length(), ' ');

	cout
		<< type << spacesType << "| "
		<< fileCount << spacesFileCount << "| "
		<< accuracy << "%" << spacesAccuracy << "| "
		<< loss << "%" << spacesLoss << "| "
		<< (timeElapsed.count() * 1000) << " ms"
		<< endl;
}
#endif

void test(MapHistos* models) {
	std::string dataset;

#ifndef _DEBUG

	cout
		<< "Testing...\n\n"
		<< "\nTYPE        | File count | accuracy | loss   | duration"
		<< "\n------------|------------|----------|--------|--------------"
		<< endl;

	dataset = "DEBUG";

#else

	dataset = "RELEASE";

#endif

	const string testPath = RESOURCES_PATH + dataset + "/" + TEST_DIR;

	tp t0 = system_clock::now();

#ifndef _DEBUG

	map<string, TestResult> modelResults;

	for(auto entry = models->begin(); entry != models->end(); ++entry) {
		tp tTestStart = system_clock::now();
		//modelResults.insert({ entry.first, testWithModels(testPath + entry.first, entry.first, models) }); // parallel
		modelResults.insert({ entry->first, testWithModels_seq(testPath + entry->first, entry->first, models) }); // sequential
		tp tTestEnd = system_clock::now();
		
		logTestResults(modelResults.at(entry->first), entry->first, tTestEnd - tTestStart);
	}

	cout << endl;

	float accuracy = 0;
	float loss = 0;
	
	for (const auto& entry : modelResults) {
		accuracy += entry.second.accuracy;
		loss += entry.second.loss;
	}

	accuracy /= modelResults.size();
	loss /= modelResults.size();

	cout
		<< "Total Accuracy : " << accuracy << "%\n"
		<< "Total Loss : " << loss << "%"
		<< endl;

#else

	const TestResult results = testWithModels_seq(testPath, "", models);

#endif

	tp t1 = system_clock::now();

	duration<float> testingTotalTime = t1 - t0;

	cout << "Testing duration : " << (testingTotalTime.count() * 1000) << " ms\n" << endl;
}




/**
 * Atomic function used by both sequential and parallel functions to process the test on an image.
 *
 * @param path - The image's path
 * @param modelsFree - The matrix of histograms that contains all resulted histograms from training images for free places.
 * @param modelsFull - The matrix of histograms that contains all resulted histograms from training images for taken places.
 */
string testFile(const string& path, MapHistos* models) {
	// create histogram for the test image
	ImageHisto hist = emptyHisto();

	processImage(path, hist);

	map<string, float> minimums;

	for (auto model = models->begin(); model != models->end(); ++model) {
		minimums.insert({ model->first, numeric_limits<float>::max() });

		for (size_t i = 0; i < model->second.size(); i++) {
			const float localSum = L1Distance(hist, model->second[i], HISTO_TOTAL_SIZE);

			if (localSum < minimums.at(model->first)) {
				minimums.at(model->first) = localSum;
			}
		}
	}

	auto min = min_element(minimums.begin(), minimums.end(),
		[](const auto& a, const auto& b) {
			return a.second < b.second;
		}
	);

	free(hist);

	return min->first;
}

TestResult testWithModels_seq(const string& path, const string& expectation, MapHistos* models) {
	const size_t fileCount = fileCountInDir(path);

	uint success = 0;

	for (const auto& entry : fs::directory_iterator(path)) {
		string filepath = entry.path().string();

		if (testFile(filepath, models) == expectation) {
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