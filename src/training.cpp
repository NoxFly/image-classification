#include "training.hpp"

using namespace std;
namespace fs = filesystem;



void trainModel_seq(const string& trainDir, const string& modelName, MapHistos* histos);


// -------------------------------------------------------------------------------------
// --------------------------------------- TRAIN ---------------------------------------
// -------------------------------------------------------------------------------------

void train(MapHistos* histos) {
	cout << "Training..." << endl << flush;

	std::string dataset;

//#ifndef _DEBUG
	dataset = "DEBUG";
//#else
	//dataset = "RELEASE";
//#endif

	const string trainPath = RESOURCES_PATH + dataset + "/" TRAIN_DIR;

	// scan dirs in trainPath and store them in a vector
	vector<string> dirs;

	for(const fs::directory_entry& entry : fs::directory_iterator(trainPath)) {
		if (entry.is_directory()) {
			dirs.push_back(entry.path().filename().string());
		}
	}

	// train each dir, each dir being a category (model)

	tp t0 = system_clock::now();

	for(const string& modelName : dirs) {
		//trainModel(trainPath, modelName, histos); // parallel
		trainModel_seq(trainPath, modelName, histos); // sequential
	}

	tp t1 = system_clock::now();

	const duration<float> trainingTime = t1 - t0;

#ifndef _DEBUG
	cout << endl;
#endif

	cout << "Training duration : " << (trainingTime.count() * 1000) << " ms\n" << endl << flush;
}







void trainModel_seq(const std::string& trainDir, const std::string& modelName, MapHistos* histos) {
	const std::string path = trainDir + modelName;

	const size_t fileCount = fileCountInDir(path);

	histos->insert({ modelName, VecHistos(fileCount) });

	uint i = 0;

	for (const auto& entry : fs::directory_iterator(path)) {
		string filepath = entry.path().string();

		histos->at(modelName)[i] = emptyHisto();

		processImage(filepath, histos->at(modelName)[i]);

		//cout << filepath << " processed" << endl;

		i++;
	}
}