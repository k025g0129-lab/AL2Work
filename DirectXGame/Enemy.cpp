#define NOMINMAX
#include "Enemy.h"
#include "KamataEngine.h"
#include <numbers> 
#include <cassert> 
#include <algorithm> 
#include "function.h"
#include "Player.h"
#include "GameScene.h"


void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3 pos) {
	assert(model);
	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;

	worldTransform_.rotation_.y = (std::numbers::pi_v<float> / 2.0f) * 3.0f;

	velocity_ = {-kWalkSpeed, 0, 0};

	walkTimer_ = 0.0f;

}

void Enemy::Update() {

	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {
		case Enemy::Behavior::kMove:
			BehaviorMoveInitialize();
		    //worldTransform_.rotation_.y = (std::numbers::pi_v<float> / 2.0f) * 3.0f;
			break;
		case Enemy::Behavior::kDeath:
			BehaviorDeathInitialize();
		    //deathParameter_ = 0;
			break;

		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	float param;
	float degree;
	switch (behavior_) {
	case Enemy::Behavior::kMove:

		walkTimer_ += 1.0f / 60.0f;
		param = std::sin((std::numbers::pi_v<float> * 2.0f) * walkTimer_ / kWalkMotionTime);
		degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
		worldTransform_.rotation_.x = RadToDeg(degree);

		// Vector3Add(worldTransform_.translation_,velocity_);
		worldTransform_.translation_.x += velocity_.x;
		break;


	case Enemy::Behavior::kDeath:

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

void Enemy::Draw() {

	model_->Draw(worldTransform_, *camera_);

}

KamataEngine::Vector3 Enemy::GetWorldPos() {

	KamataEngine::Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;

}

AABB Enemy::GetAABB() {

	KamataEngine::Vector3 worldPos = GetWorldPos();
	AABB aabb;
	aabb.min = {worldPos.x - kWidth, worldPos.y - kHeigth, worldPos.z - kWidth};
	aabb.max = {worldPos.x + kWidth, worldPos.y + kHeigth, worldPos.z + kWidth};

	return aabb; 



}



void Enemy::OnCollision(const Player* player) {
   //(void)player; 

	if (behavior_ == Behavior::kDeath) {
	   return;
	}

   if (player->isDash()) {
		isCollisionDisabled_ = true;
		behaviorRequest_ = Behavior::kDeath;
   }

   KamataEngine::Vector3 effectPos = (Vector3Add(worldTransform_.translation_, player->GetWorldTransform()));
   effectPos.x /= 2.0f;
   effectPos.y /= 2.0f;
   effectPos.z /= 2.0f;

   gameScene_->CreateHitEffect(effectPos);

}

void Enemy::BehaviorMoveInitialize() {
	worldTransform_.rotation_.x = 0.0f;
	worldTransform_.rotation_.y = (std::numbers::pi_v<float> / 2.0f) * 3.0f;
	worldTransform_.rotation_.z = 0.0f;

}

void Enemy::BehaviorDeathInitialize() { 
	deathParameter_ = 0; 
}
