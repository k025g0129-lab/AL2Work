#include "HitEffect.h"
#include <numbers> 
#include <cassert> 
#include "function.h"
KamataEngine::Model* HitEffect::model_ = nullptr ;
KamataEngine::Camera* HitEffect::camera_ = nullptr;

HitEffect::~HitEffect() { 
	delete model_;
	delete camera_;
}

void HitEffect::Initialize(KamataEngine::Vector3 pos) { 
	circleworldTransform_.Initialize();
	circleworldTransform_.scale_ = {0.5f,0.5f,0.5f};
	circleworldTransform_.translation_ = pos;
	ellipseDeath.Initialize();

	for (KamataEngine::WorldTransform& worldTransform : ellipseWorldTransform_) {

		worldTransform.scale_ = {0.2f,1.3f,1.0f};
		worldTransform.rotation_ = { 0.0f, 0.0f, RandomFloat(-(std::numbers::pi_v<float> / 2.0f), std::numbers::pi_v<float> / 2.0f) };
		worldTransform.translation_ = pos;

		worldTransform.Initialize();
	}

}

void HitEffect::Update() {


	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {
		case HitEffect::Behavior::kDeathAnimation:
			break;
		case HitEffect::Behavior::kDeath:
			deathPhase_ = DeathPhase::kExpansion;
			deathParameter_ = 0;
			break;
		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case HitEffect::Behavior::kAlive:
		break;
	case HitEffect::Behavior::kDeathAnimation:

		DeathAnimationUpdate();



		break;
	case HitEffect::Behavior::kDeath:
		break;
	default:
		break;
	}


	MakeAffineMatrix(&circleworldTransform_); 

	for (KamataEngine::WorldTransform& worldTransform : ellipseWorldTransform_) {
		MakeAffineMatrix(&worldTransform); 
	}

}

void HitEffect::Draw() { 
	
	switch (behavior_) {

	case HitEffect::Behavior::kDeathAnimation:
		model_->Draw(circleworldTransform_, *camera_, &ellipseDeath); 

		for (auto& worldTransform : ellipseWorldTransform_) {
			model_->Draw(worldTransform, *camera_, &ellipseDeath);
		}
		break;
	case HitEffect::Behavior::kDeath:
		break;

	}


}

HitEffect* HitEffect::Create(KamataEngine::Vector3 pos) {
	
	HitEffect* instance = new HitEffect();

	assert(instance);

	instance->Initialize(pos);
	
	
	return instance; 


}

void HitEffect::SetIsDead() { 
	behaviorRequest_ = Behavior::kDeathAnimation;

}



void HitEffect::DeathAnimationUpdate() {

	for (KamataEngine::WorldTransform& worldTransform : ellipseWorldTransform_) {

		switch (deathPhase_) {
		case HitEffect::DeathPhase::kExpansion:
			deathParameter_++;
			worldTransform.scale_.y += 0.1f;
			circleworldTransform_.scale_.x += 0.02f;
			circleworldTransform_.scale_.y += 0.02f;
			circleworldTransform_.scale_.z += 0.02f;

			if ( deathParameter_ >=expansionTime) {
				deathPhase_ = DeathPhase::kFadeOut;
				deathParameter_ = 0;
			}
			break;
		case HitEffect::DeathPhase::kFadeOut:
			deathParameter_++;
			ellipseDeath.SetColor({1.0f, 1.0f, 1.0f, EaseOutSine(1.0f, 0.0f, static_cast<float>(deathParameter_), static_cast<float>(fadeOutTime))});
			
			if ( deathParameter_ >=fadeOutTime) {
				deathParameter_ = 0;
				deathPhase_ = DeathPhase::kFadeOut;
				behavior_ = Behavior::kDeath;
			}

			break;
		default:
			break;
		}
	}

}
