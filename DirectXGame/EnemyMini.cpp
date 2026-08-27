#define NOMINMAX
#include "EnemyMini.h"
#include "KamataEngine.h"
#include <numbers> 
#include <cassert> 
#include <algorithm> 
#include "function.h"
#include "Player.h"
#include "GameScene.h"
#include "MapChipField.h"


void EnemyMini::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3 pos) {
	assert(model);
	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;

	worldTransform_.rotation_.y = (std::numbers::pi_v<float> / 2.0f) * 3.0f;

	velocity_ = {-kWalkSpeed, 0, 0};

	walkTimer_ = 0.0f;

	startRot = 0.0f;
	targetRot = 0.0f;
	turnParameter_ = 0;

}

void EnemyMini::Update() {

	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {
		case EnemyMini::Behavior::kMove:
			BehaviorMoveInitialize();
		    //worldTransform_.rotation_.y = (std::numbers::pi_v<float> / 2.0f) * 3.0f;
			break;

		case EnemyMini::Behavior::kTurn:
			BehaviorTurnInitialize();
			break;

		case EnemyMini::Behavior::kDeath:
			BehaviorDeathInitialize();
		    //deathParameter_ = 0;
			break;

		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	float param;
	float degree;
	switch (behavior_) {
	case EnemyMini::Behavior::kMove:

		walkTimer_ += 1.0f / 60.0f;
		param = std::sin((std::numbers::pi_v<float> * 2.0f) * walkTimer_ / kWalkMotionTime);
		degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
		worldTransform_.rotation_.x = RadToDeg(degree);

		// Vector3Add(worldTransform_.translation_,velocity_);


		if (ShouldTurn()) {
			behaviorRequest_ = Behavior::kTurn;
		} else {
			worldTransform_.translation_.x += velocity_.x;
		}

		break;


	case EnemyMini::Behavior::kTurn:
		turnParameter_++;
		worldTransform_.rotation_.y = EaseInSine(startRot,targetRot,float(turnParameter_),float(turnTime));

		if (turnParameter_ >= turnTime) {
			worldTransform_.rotation_.y = targetRot;
			behaviorRequest_ = Behavior::kMove;
		}

		break;


	case EnemyMini::Behavior::kDeath:

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

void EnemyMini::Draw() {

	model_->Draw(worldTransform_, *camera_);

}

KamataEngine::Vector3 EnemyMini::GetWorldPos() {

	KamataEngine::Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;

}

AABB EnemyMini::GetAABB() {

	KamataEngine::Vector3 worldPos = GetWorldPos();
	AABB aabb;
	aabb.min = {worldPos.x - kWidth, worldPos.y - kHeigth, worldPos.z - kWidth};
	aabb.max = {worldPos.x + kWidth, worldPos.y + kHeigth, worldPos.z + kWidth};

	return aabb; 



}



void EnemyMini::OnCollision() {
   //(void)player; 

	if (behavior_ == Behavior::kDeath) {
	   return;
	}

	isCollisionDisabled_ = true;
	behaviorRequest_ = Behavior::kDeath;
   

   //KamataEngine::Vector3 effectPos = (Vector3Add(worldTransform_.translation_, player->GetWorldTransform()));
   //effectPos.x /= 2.0f;
   //effectPos.y /= 2.0f;
   //effectPos.z /= 2.0f;

   gameScene_->CreateHitEffect(worldTransform_.translation_);

}

void EnemyMini::OnAttackCollision() {

	if (behavior_ == Behavior::kDeath) {
		return;
	}

	isCollisionDisabled_ = true;
	behaviorRequest_ = Behavior::kDeath;

	gameScene_->CreateHitEffect(worldTransform_.translation_);

}

void EnemyMini::BehaviorMoveInitialize() {
	worldTransform_.rotation_.x = 0.0f;
	worldTransform_.rotation_.z = 0.0f;

}

void EnemyMini::BehaviorTurnInitialize() { 
	velocity_.x *= -1.0f;
	startRot = worldTransform_.rotation_.y;
	
	if (velocity_.x > 0.0f) {
		targetRot = std::numbers::pi_v<float> / 2.0f;
	} else {
		targetRot = (std::numbers::pi_v<float> / 2.0f) * 3.0f;
	}

	turnParameter_ = 0;

}

void EnemyMini::BehaviorDeathInitialize() { 
	deathParameter_ = 0; 
}

bool EnemyMini::ShouldTurn() { 

	float direction = 1.0f;

	if (velocity_.x < 0.0f) {
		direction = -1.0f;
	}

	
	KamataEngine::Vector3 frontPos = worldTransform_.translation_;
	frontPos.x += direction * (kWidth + 0.1f);


	auto frontIndex = mapChipField_->GetMapChipIndexSetByPos(frontPos);

	if (mapChipField_->GetMapChipTypeByIndex(frontIndex.xIndex, frontIndex.yIndex) == MapChipType::kBlock) {

		return true;
	}


	KamataEngine::Vector3 footPos = frontPos;

	footPos.y -= kHeigth + 0.2f;

	auto footIndex = mapChipField_->GetMapChipIndexSetByPos(footPos);

	if (mapChipField_->GetMapChipTypeByIndex(footIndex.xIndex, footIndex.yIndex) != MapChipType::kBlock) {
		return true;
	}

	return false;

}
