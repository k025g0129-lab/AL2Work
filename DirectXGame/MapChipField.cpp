#include "MapChipField.h"
#include <map>
#include <fstream>
#include <sstream>
#include <numbers> 
#include <cassert> 


namespace {

	std::map<char, MapChipType> mapChipTypeTable = {
		//{"0",MapChipType::kBlank},
		{'B', MapChipType::kBlock},
		{'P', MapChipType::kPlayer},
		{'E', MapChipType::kEnemy},
	};

}

void MapChipField::ResetMapChipData() { 
    
    mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipDataUnit>& mapChipTypeLine : mapChipData_.data) {
		mapChipTypeLine.resize(kNumBlockHorizontal);

	}

}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {

	ResetMapChipData();

	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	std::stringstream mapChipCsv;
	mapChipCsv << file.rdbuf();

	file.close();

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		std ::string line;
		getline(mapChipCsv, line);

		std ::istringstream lineStream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {

			std ::string word;
			std::getline(lineStream, word, ',');
			
			if (word.empty()) {
				continue;
			}

			if (!mapChipTypeTable.contains(word[kChipType])) {
				continue;
			}

			mapChipData_.data[i][j].type = mapChipTypeTable[word[kChipType]];

			if (word.size() <= kChipSubID) {
				continue;
			}

			mapChipData_.data[i][j].subID = static_cast<uint8_t>(word[kChipSubID] - '0');	

		}
	}

}

	MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) { 

	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex) {
		return MapChipType :: kBlank;
	}
	if (yIndex < 0 || kNumBlockVirtical - 1 < yIndex) {
		return MapChipType :: kBlank;
	}

	return mapChipData_.data[yIndex][xIndex].type;

}

uint8_t MapChipField::GetMapChipSubIDByIndex(uint32_t xIndex, uint32_t yIndex) { 

	//if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex) {
	//	return MapChipType ::kBlank;
	//}
	//if (yIndex < 0 || kNumBlockVirtical - 1 < yIndex) {
	//	return MapChipType ::kBlank;
	//}

	return mapChipData_.data[yIndex][xIndex].subID;
}

KamataEngine::Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) { 

	return KamataEngine::Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - 1 - yIndex), 0);

}

MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPos(const KamataEngine::Vector3& pos) { 

	IndexSet indexSet;
	indexSet.xIndex = uint32_t((pos.x + kBlockWidth / 2.0f) / kBlockWidth);
	indexSet.yIndex = uint32_t((pos.y + kBlockHeight / 2.0f) / kBlockHeight);
	indexSet.yIndex = kNumBlockVirtical - 1 - indexSet.yIndex;

	return indexSet;
}



uint32_t MapChipField::GetNumBlockVirtical() {

	return kNumBlockVirtical; 

}

uint32_t MapChipField::GetNumBlockHorizontal() {

	return kNumBlockHorizontal;

}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) { 

	KamataEngine::Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y + kBlockHeight / 2.0f;
	rect.top = center.y - kBlockHeight / 2.0f;

	return rect;
}
