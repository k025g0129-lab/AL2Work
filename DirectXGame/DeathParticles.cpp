#define NOMINMAX
#include "DeathParticles.h"
#include "KamataEngine.h"
#include <numbers> 
#include <cassert> 
#include <algorithm> 
#include "function.h"

void DeathParticles::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3 pos) {

	assert(model);
	model_ = model;
	camera_ = camera;

	for (KamataEngine::WorldTransform& worldTransform : worldTransform_) {
		worldTransform.Initialize();
		worldTransform.translation_ = pos;
	}

	objectColor_.Initialize();
	color_ = {1, 1, 1, 1};



}

void DeathParticles::Update() {

	//デスパーティクル
	counter_ += 1.0f / 60.0f;

	if (counter_ >= kDuration) {
		counter_ = kDuration;
		isFinished_ = true;
	}
	color_.w-= 0.01f;
	color_.w = std::clamp(color_.w,0.0f,1.0f);	
	objectColor_.SetColor(color_);

	if (isFinished_) {
		return;
	}

	for (uint32_t i = 0; i < kNumParticles; i++) {
		KamataEngine::Vector3 velocity = {kSpeed, 0, 0};

		float angle = kAngleUnit * i;

		KamataEngine::Matrix4x4 matrixRot = MakeRotateZMatrix(angle);

		velocity = TransformM4x4ToV3(velocity,matrixRot);

		worldTransform_[i].translation_.x += velocity.x;
		worldTransform_[i].translation_.y += velocity.y;
		worldTransform_[i].translation_.z += velocity.z;

	}



	for (KamataEngine::WorldTransform& worldTransform : worldTransform_) {
		MakeAffineMatrix(&worldTransform);
	}

	
}

void DeathParticles::Draw() {

	if (isFinished_) {
		return;
	}

	for (KamataEngine::WorldTransform& worldTransform : worldTransform_) {
		model_->Draw(worldTransform, *camera_, & objectColor_);
	}
	

}
