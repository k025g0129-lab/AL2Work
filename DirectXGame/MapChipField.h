#pragma once
#include "KamataEngine.h"

enum class MapChipType {
	kBlank,
	kBlock,
	kPlayer,
	kEnemy,
};

enum MapChipCharIndex {
	kChipType = 0,
	kChipSubID = 1,
};

struct MapChipDataUnit {
	MapChipType type;
	uint8_t subID;

};

struct MapChipData {
	std::vector<std::vector<MapChipDataUnit>> data;
};

class MapChipField {
public:

	struct IndexSet {
	
		uint32_t xIndex;
		uint32_t yIndex;
	};

	struct Rect {
		float left;
		float right;
		float bottom;
		float top;
	};


public:
	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	uint8_t GetMapChipSubIDByIndex(uint32_t xIndex, uint32_t yIndex);
	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	IndexSet GetMapChipIndexSetByPos(const KamataEngine::Vector3& pos);

	uint32_t GetNumBlockVirtical();
	uint32_t GetNumBlockHorizontal();
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

private:
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;


	MapChipData mapChipData_;
};
