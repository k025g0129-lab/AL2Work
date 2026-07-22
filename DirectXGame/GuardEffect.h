#pragma once
#include "KamataEngine.h"

class GuardEffect {
public:
	enum class Behavior {
		kAlive,
		kKnockbackAnimation,
		kDeath,
		kUnknown,
	};

	enum class DeathPhase {
		kExpansion,
		kFadeOut,

	};

public:
	~GuardEffect();

	void Initialize(KamataEngine::Vector3 pos);

	void Update();

	void Draw();

	static void SetModel(KamataEngine::Model* model) { model_ = model; };
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; };
	static GuardEffect* Create(KamataEngine::Vector3 pos);

	void SetIsKnockback();

	void KnockbackAnimationUpdate();

private:
	KamataEngine::WorldTransform worldTransform_;
	static KamataEngine::Model* model_;
	static KamataEngine::Camera* camera_;
	uint32_t textureHandle_ = 0u;

	KamataEngine::WorldTransform circleworldTransform_;

	Behavior behavior_ = Behavior::kAlive;
	Behavior behaviorRequest_ = Behavior::kAlive;
	uint32_t deathParameter_ = 0;

	DeathPhase deathPhase_ = DeathPhase::kExpansion;
	uint32_t expansionTime = 60;
	uint32_t fadeOutTime = 120;
	KamataEngine::ObjectColor ellipseDeath;

	// ガード
	bool isGuard_ = false;

};
