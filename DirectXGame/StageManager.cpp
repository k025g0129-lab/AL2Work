#include "StageManager.h"
#include <fstream>
#include <sstream>

#include <cassert>


void StageManager::LoadStageDataCsv() {
	
	const std::string filePath = "Resources/stageDatas.csv";

	std::ifstream file;
	file.open(filePath);
	assert(file.is_open() && "");

	std::stringstream stageDataCsv;
	stageDataCsv << file.rdbuf();

	file.close();


	std ::string line;
	while (std::getline(stageDataCsv, line)) {

		StageData stageData;

		std ::stringstream lineStream(line);
		std::string word;

		std::getline(lineStream, word, ',');
		stageData.name = word;

		std::getline(lineStream, word, ',');
		stageData.timeLimit = std::stoi( word);

		stageDatas_.push_back(stageData);
	
	}
}
