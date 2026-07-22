#include "GuardEffect.h"
#include "function.h"
#include <cassert>
#include <numbers>

KamataEngine::Model* GuardEffect::model_ = nullptr;
KamataEngine::Camera* GuardEffect::camera_ = nullptr;

GuardEffect::~GuardEffect() {


}

void GuardEffect::Initialize(KamataEngine::Vector3 pos) {
	circleworldTransform_.Initialize();
	circleworldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
	circleworldTransform_.translation_ = pos;
	ellipseDeath.Initialize();

}

void GuardEffect::Update() {

	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {
		case GuardEffect::Behavior::kKnockbackAnimation:
			break;
		case GuardEffect::Behavior::kDeath:
			deathPhase_ = DeathPhase::kExpansion;
			deathParameter_ = 0;
			break;
		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case GuardEffect::Behavior::kAlive:
		break;
	case GuardEffect::Behavior::kKnockbackAnimation:

		KnockbackAnimationUpdate();

		break;
	case GuardEffect::Behavior::kDeath:
		break;
	default:
		break;
	}

	MakeAffineMatrix(&circleworldTransform_);

}

void GuardEffect::Draw() {

	switch (behavior_) {

	case GuardEffect::Behavior::kKnockbackAnimation:
		model_->Draw(circleworldTransform_, *camera_, &ellipseDeath);

		break;
	case GuardEffect::Behavior::kDeath:
		break;
	}
}

GuardEffect* GuardEffect::Create(KamataEngine::Vector3 pos) {

	GuardEffect* instance = new GuardEffect();

	assert(instance);

	instance->Initialize(pos);

	return instance;
}


void GuardEffect::SetIsKnockback() {
	behaviorRequest_ = Behavior::kKnockbackAnimation; 
}

void GuardEffect::KnockbackAnimationUpdate() {

	switch (deathPhase_) {
	case GuardEffect::DeathPhase::kExpansion:
		deathParameter_++;	
		circleworldTransform_.scale_.x += 0.02f;
		circleworldTransform_.scale_.y += 0.02f;
		circleworldTransform_.scale_.z += 0.02f;

		if (deathParameter_ >= expansionTime) {
			deathPhase_ = DeathPhase::kFadeOut;
			deathParameter_ = 0;
		}
		break;
	case GuardEffect::DeathPhase::kFadeOut:
		deathParameter_++;
		ellipseDeath.SetColor({1.0f, 1.0f, 1.0f, EaseOutSine(1.0f, 0.0f, static_cast<float>(deathParameter_), static_cast<float>(fadeOutTime))});

		if (deathParameter_ >= fadeOutTime) {
			deathParameter_ = 0;
			deathPhase_ = DeathPhase::kFadeOut;
			behavior_ = Behavior::kDeath;
		}

		break;
	default:
		break;
	}
	
}
