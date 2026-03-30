#pragma once
#include "KamataEngine.h"

class HitEffect {
public:
	enum class Behavior {
		kAlive, 
		kDeathAnimation, 
		kDeath,
		kUnknown,
	};

	enum class DeathPhase {
		kExpansion,
		kFadeOut,

	};


public:
	~HitEffect();

	void Initialize(KamataEngine::Vector3 pos);

	void Update();
	
	void Draw();

	static void SetModel(KamataEngine::Model* model) { model_ = model; };
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; };
	static HitEffect* Create(KamataEngine::Vector3 pos);
	void SetIsDead();

	void DeathAnimationUpdate();

private:
	KamataEngine::WorldTransform worldTransform_;
	static KamataEngine::Model* model_;
	static KamataEngine::Camera* camera_;
	uint32_t textureHandle_ = 0u;

	KamataEngine::WorldTransform circleworldTransform_;

	std::array<KamataEngine::WorldTransform, 2> ellipseWorldTransform_;

	Behavior behavior_ = Behavior::kAlive;
	Behavior behaviorRequest_ = Behavior::kAlive;
	uint32_t deathParameter_ = 0;

	DeathPhase deathPhase_ = DeathPhase::kExpansion;
	uint32_t expansionTime = 60;
	uint32_t fadeOutTime = 120;
	KamataEngine::ObjectColor ellipseDeath;
};
	