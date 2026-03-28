#pragma once
#include"KamataEngine.h"
#include "Fade.h"

class TitleScene {
public:
	enum class Phase {
		kFadeIn,
		kMain,
		kFadeOut,
	};


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

	Fade* fade_ = nullptr;	
	Phase phase_ = Phase::kFadeIn;


};
