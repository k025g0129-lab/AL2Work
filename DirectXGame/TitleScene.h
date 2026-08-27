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

	KamataEngine::Model* modelTitleLogo_ = nullptr;
	KamataEngine::WorldTransform worldTransformTitleLogo_;

	float titleLogoTimer_ = 0.0f;

	// 基準位置
	float titleLogoBaseY_ = 2.0f;

	// 上下する幅
	static inline const float kTitleLogoFloatRange = 0.3f;

	// 上下する速さ
	static inline const float kTitleLogoFloatSpeed = 2.0f;

	bool finished_ = false;

	Fade* fade_ = nullptr;	
	Phase phase_ = Phase::kFadeIn;

	uint32_t pressSpaceTextureHandle_ = 0;
	KamataEngine::Sprite* pressSpaceSprite_ = nullptr;

	uint32_t zimenTextureHandle_ = 0;
	KamataEngine::Sprite* zimenSprite_ = nullptr;


};
