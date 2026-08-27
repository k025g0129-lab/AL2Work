#define NOMINMAX
#include "ShieldEnemy.h"
#include "KamataEngine.h"
#include <numbers> 
#include <cassert> 
#include <algorithm> 
#include "function.h"
#include "Player.h"
#include "GameScene.h"


void ShieldEnemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3 pos) {
	assert(model);
	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;

	worldTransform_.rotation_.y = (std::numbers::pi_v<float> / 2.0f) * 3.0f;

	velocity_ = {-kWalkSpeed, 0, 0};

	walkTimer_ = 0.0f;


}

void ShieldEnemy::Update() {

	if (isKnockbackRequest) {

		behaviorRequest_ = Behavior::kKnockback;

		isKnockbackRequest = false;
	}


	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {

		case ShieldEnemy::Behavior::kMove:
			BehaviorMoveInitialize();
			// worldTransform_.rotation_.y = (std::numbers::pi_v<float> / 2.0f) * 3.0f;
			break;

		case ShieldEnemy::Behavior::kKnockback:
			BehaviorKnockbackInitialize();
			break;

		case ShieldEnemy::Behavior::kDeath:
			BehaviorDeathInitialize();
			// deathParameter_ = 0;
			break;

		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	float param;
	float degree;
	switch (behavior_) {
	case ShieldEnemy::Behavior::kMove:

		walkTimer_ += 1.0f / 60.0f;
		param = std::sin((std::numbers::pi_v<float> * 2.0f) * walkTimer_ / kWalkMotionTime);
		degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
		worldTransform_.rotation_.y = RadToDeg(degree);

		// Vector3Add(worldTransform_.translation_,velocity_);
		worldTransform_.translation_.x += velocity_.x;
		break;


	case ShieldEnemy::Behavior::kKnockback:

		KnockbackUpdata();

		break;

	case ShieldEnemy::Behavior::kDeath:

		deathParameter_++;
		worldTransform_.rotation_.x += 1.0f;
		worldTransform_.rotation_.y += 1.0f;
		worldTransform_.rotation_.z += 1.0f;
		if (deathParameter_ >= deathTime) {

			isDead_ = true;
		}

		break;
	}

	MakeAffineMatrix(&worldTransform_);


}

void ShieldEnemy::KnockbackUpdata() {
	knockbackTimer_++;

	switch (knockbackStatus) {
	case ShieldEnemy::KnockbackStatus::kBackward:

		worldTransform_.rotation_.x = EaseInSine(0.0f, -(std::numbers::pi_v<float> / 2.0f) * 1.0f, knockbackTimer_, kKnockbackMotionTime);


		if (knockbackTimer_ >= kKnockbackMotionTime) {
			knockbackTimer_ = 0.0f;
			knockbackStatus = KnockbackStatus::kForward;
		}

		break;
	case ShieldEnemy::KnockbackStatus::kForward:

		worldTransform_.rotation_.x = EaseInSine(-(std::numbers::pi_v<float> / 2.0f) * 1.0f, 0.0f, knockbackTimer_, kKnockbackMotionTime);


		if (knockbackTimer_ >= kKnockbackMotionTime) {

			knockbackTimer_ = 0.0f;
			knockbackStatus = KnockbackStatus::kBackward;

			behaviorRequest_ = Behavior::kMove;
		}

		break;

	}


}

void ShieldEnemy::Draw() {

		model_->Draw(worldTransform_, *camera_);

}

KamataEngine::Vector3 ShieldEnemy::GetWorldPos() { 
	
	KamataEngine::Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB ShieldEnemy::GetAABB() {

	KamataEngine::Vector3 worldPos = GetWorldPos();
	AABB aabb;
	aabb.min = {worldPos.x - kWidth, worldPos.y - kHeigth, worldPos.z - kWidth};
	aabb.max = {worldPos.x + kWidth, worldPos.y + kHeigth, worldPos.z + kWidth};

	return aabb;
}

void ShieldEnemy::OnCollision(Player* player) {

	if (behavior_ == Behavior::kDeath) {
		return;
	}

	if (player->isDash()) {

		//向き合っているか
		if (player->GetLRDirection() != lrDirection_) {
			KamataEngine::Vector3 effectPos = (Vector3Add(worldTransform_.translation_, player->GetWorldTransform().translation_));
			effectPos.x /= 2.0f;
			effectPos.y /= 2.0f;
			effectPos.z /= 2.0f;
			gameScene_->CreateGuardEffect(effectPos);
			isBehaviorRequestKnockback();

			player->isBehaviorRequestKnockback();
			
			return;
		}

		isCollisionDisabled_ = true;
		behaviorRequest_ = Behavior::kDeath;
	}

	KamataEngine::Vector3 effectPos = (Vector3Add(worldTransform_.translation_, player->GetWorldTransform().translation_));
	effectPos.x /= 2.0f;
	effectPos.y /= 2.0f;
	effectPos.z /= 2.0f;

	gameScene_->CreateHitEffect(effectPos);


}

void ShieldEnemy::BehaviorMoveInitialize() {
	worldTransform_.rotation_.x = 0.0f;
	worldTransform_.rotation_.y = (std::numbers::pi_v<float> / 2.0f) * 3.0f;
	worldTransform_.rotation_.z = 0.0f;

}

void ShieldEnemy::BehaviorKnockbackInitialize() {

	worldTransform_.rotation_.x = 0.0f;
	worldTransform_.rotation_.y = (std::numbers::pi_v<float> / 2.0f) * 3.0f;
	worldTransform_.rotation_.z = 0.0f;

	knockbackStatus = KnockbackStatus::kBackward;
	knockbackTimer_ = 0.0f;

}

void ShieldEnemy::BehaviorDeathInitialize() {
	deathParameter_ = 0; 

}
