#include "training.hpp"

using namespace std;
namespace fs = filesystem;


// -------------------------------------------------------------------------------------
// --------------------------------------- TRAIN ---------------------------------------
// -------------------------------------------------------------------------------------

void train_atom(const string& trainDir, const std::string& type, void (*fn)(const string&, VecHistos&), VecHistos& histos) {
	cout << "  Training " << type << " places..." << flush;

	tp t0Train = system_clock::now();
	fn(trainDir + type, histos);
	tp t1Train = system_clock::now();

	const duration<double> trainingTime = t1Train - t0Train;

	cout << " Done (" << (trainingTime.count() * 1000) << " ms)" << endl << flush;

#ifdef _DEBUG
	cout << endl;
#endif
}

void train(const bool multithreaded, const string& dataset, VecHistos& histosTrainFree, VecHistos& histosTrainFull) {
	auto trainFn = multithreaded ? train_par : train_seq;

	cout << "Training..." << endl << flush;

	const string trainPath = RESOURCES_PATH "Dataset" + dataset + "/" TRAIN_DIR;

	tp t0 = system_clock::now();

	train_atom(trainPath, "Free", trainFn, histosTrainFree);
	train_atom(trainPath, "Full", trainFn, histosTrainFull);

	tp t1 = system_clock::now();

	const duration<double> trainingTime = t1 - t0;

#ifdef _DEBUG
	cout << endl;
#endif

	cout << "Training duration : " << (trainingTime.count() * 1000) << " ms\n" << endl << flush;
}




/**
 * Training function SEQUENTIAL execution
 * Proceed all the files one per one on one thread.
 *
 * @param path - The directory path that contains all images to be proceed.
 * @param histos - The matrix of histograms that will contain all resulted histograms from given images.
 */
void train_seq(const string& path, VecHistos& histos) {
	const size_t fileCount = fileCountInDir(path);

	histos.resize(fileCount);

	uint i = 0;

	for (const auto& entry : fs::directory_iterator(path)) {
		string filepath = entry.path().string();

		processImage(filepath, histos[i]);

		i++;
	}
}





void processTrainFilePar(stack<string>& fileStack, VecHistos& histos, mutex& mtx, condition_variable& cv) {
	while (true) {
		string currentFile;

		{
			unique_lock<mutex> lock(mtx);

			if (fileStack.empty()) {
				break;
			}

			currentFile = fileStack.top();
			fileStack.pop();

#ifdef _DEBUG
			cout << "(" << fileStack.size() << ")" << "processImage : " << currentFile << endl;
#endif
		}

		ImageHisto h;
		processImage(currentFile, h);

		{
			unique_lock<mutex> lock(mtx);
			
			histos[fileStack.size()] = h;
		}

		cv.notify_one();
	}
}


/**
 * Training function PARALLEL execution
 * Detects all available cores of the CPU
 * Separates image processing over all.
 *
 * @param path - The directory path that contains all images to be proceed.
 * @param histos - The matrix of histograms that will contain all resulted histograms from given images.
 */
void train_par(const string& path, VecHistos& histos) {
	const uint fileCount = (uint)fileCountInDir(path);

	histos.resize(fileCount);

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
	std::cout << std::endl;
#endif

	for (uint i = 0; i < coreCount; ++i) {
		threads.emplace_back(processTrainFilePar, ref(fileStack), ref(histos), ref(mtx), ref(cv));
	}

	uint i = 0;

	for (thread& t : threads) {
		t.join();
	}
}



