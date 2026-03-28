#pragma once
#include "KamataEngine.h"
#include <array> 
#include <numbers> 

class DeathParticles {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3 pos);

	void Update();

	void Draw();


	bool GetIsFinished() const { return isFinished_; };

private:
	// 初期設定
	//KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	uint32_t textureHandle_ = 0u;

	//ロックマンデス
	static inline const uint32_t kNumParticles = 8;
	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransform_;

	static inline const float kDuration = 1.0f;
	static inline const float kSpeed = 0.05f;
	static inline const float kAngleUnit = (std::numbers::pi_v<float> * 2.0f) / float(kNumParticles);

	int isFinished_ = false;
	float counter_ = 0.0f;

	//デスフェードアウト
	KamataEngine::ObjectColor objectColor_;
	KamataEngine::Vector4 color_;

};