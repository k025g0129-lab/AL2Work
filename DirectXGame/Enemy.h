#pragma once
#include "KamataEngine.h"

class Enemy {
public:

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3 pos);

	void Update();

	void Draw();


private:
	//初期設定
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	uint32_t textureHandle_ = 0u;

	//移動
	KamataEngine::Vector3 velocity_ = {};
	static inline const float kWalkSpeed = 0.05f;

	//アニメーション
	static inline const float kWalkMotionAngleStart = 60.0f;
	static inline const float kWalkMotionAngleEnd = -60.0f;
	static inline const float kWalkMotionTime = 1.0f;

	float walkTimer_ = 0.0f;


};
