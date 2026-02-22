#define NOMINMAX
#include "Enemy.h"
#include "KamataEngine.h"
#include <numbers> 
#include <cassert> 
#include <algorithm> 
#include "function.h"

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

	walkTimer_ += 1.0f / 60.0f;
	float param = std::sin((std::numbers::pi_v<float> * 2.0f) * walkTimer_ / kWalkMotionTime);
	float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.x = RadToDeg(degree);


	//Vector3Add(worldTransform_.translation_,velocity_);
	worldTransform_.translation_.x += velocity_.x;

	MakeAffineMatrix(&worldTransform_);

}

void Enemy::Draw() {

	model_->Draw(worldTransform_, *camera_);
}
