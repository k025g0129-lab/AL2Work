#pragma once
#include"KamataEngine.h"

class TitleScene {
public:

	~TitleScene();
	void Initialize();

	void Update();

	void Draw();


	bool GetIsFinished() const { return finished_; };

private:

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera camera_;
	uint32_t textureHandle_ = 0u;


	bool finished_ = false;
};
