#define NOMINMAX
#include "CameraController.h"
#include <cassert> 
#include "Player.h"
#include "KamataEngine.h"
#include "function.h"

void CameraController::Initialize(KamataEngine::Camera* camera) { 
	
	camera_ = camera;


}

void CameraController::Update() { 
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	/* camera_->translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;	
	camera_->translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	camera_->translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;*/


	targetCameraPos.x = targetWorldTransform.translation_.x + targetOffset_.x + target_->GetVelocity().x * kVelocityBias;
	targetCameraPos.y = targetWorldTransform.translation_.y + targetOffset_.y + target_->GetVelocity().x * kVelocityBias;
	targetCameraPos.z = targetWorldTransform.translation_.z + targetOffset_.z + target_->GetVelocity().x * kVelocityBias;

	camera_->translation_ = Lerp(camera_->translation_,targetCameraPos,kInterpolationRate);


	camera_->translation_.x = std::max(camera_->translation_.x, target_->GetWorldTransform().translation_.x + margin.left);
	camera_->translation_.x = std::min(camera_->translation_.x, target_->GetWorldTransform().translation_.x + margin.right);
	camera_->translation_.y = std::max(camera_->translation_.y, target_->GetWorldTransform().translation_.y + margin.bottom);
	camera_->translation_.y = std::min(camera_->translation_.y, target_->GetWorldTransform().translation_.y + margin.top);	


	camera_->translation_.x = std::max(camera_->translation_.x, movableArea_.left);
	camera_->translation_.x = std::min(camera_->translation_.x, movableArea_.right);
	camera_->translation_.y = std::max(camera_->translation_.y, movableArea_.bottom);
	camera_->translation_.y = std::min(camera_->translation_.y, movableArea_.top);



	camera_->UpdateMatrix();
}

void CameraController::Draw() {


}

void CameraController::Reset() {

	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	camera_->translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	camera_->translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	camera_->translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;

}
