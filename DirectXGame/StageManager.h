#pragma once
#include<string>
#include<vector>
#include <cassert> 

class StageManager {
public:
		
	struct StageData {
		std::string name;
		int32_t timeLimit;
	};

	void LoadStageDataCsv();
	const StageData& GetStageData(int32_t index) const { 
		assert(index);
		return stageDatas_[index];
	
	}

	void SetCurrentStageIndex(int32_t index) { 
		assert(index);
		currentStageIndex_ = index;
	
	};

	int32_t GetCurrentStageIndex() const { return currentStageIndex_; };

	const StageData& GetCurrentStageData() const { 
		return GetStageData(currentStageIndex_);
	};

	private:

		std::vector<StageData> stageDatas_;
	    int32_t currentStageIndex_ = 0;

};
