#define NOMINMAX
#include "Player.h"
#include "KamataEngine.h"
#include <numbers> 
#include <cassert> 
#include <algorithm> 
#include "function.h"


Player::Player() { 

}

Player::~Player() {

}

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera,const KamataEngine::Vector3 pos) { 

	assert(model);
	model_ = model;		
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;
		
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

}

void Player::Update() {
	
	bool landing = false;

	if (velocity_.y < 0) {
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}


	//地についているか
	if (onGround_) {

		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}

		//左右移動
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT) || KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {
	
			KamataEngine::Vector3 acceleration = {};

			if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT)) {

				if (velocity_.x <0.0f) {
					velocity_.x *= (1.0f - kAcceleration);
				}

				acceleration.x += kAcceleration;

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
		
			} else if (KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {

				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAcceleration);
				}

				acceleration.x -= kAcceleration;
		
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}

			}

			velocity_.x += acceleration.x;
			velocity_.y += acceleration.y;
			velocity_.z += acceleration.z;

			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
	
		} else {
			velocity_.x *= (1.0f - kAcceleration);
		}

		if (KamataEngine::Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_.y += kJumpAcceleration;
		}

	} else {
		velocity_.y += KamataEngine::Vector3(0, -kGravityAcceleration, 0).y;
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
        
		if (landing) {
			worldTransform_.translation_.y = 1.0f;
			velocity_.x *= (1.0f - kAttenuation);
			velocity_.y = 0.0f;
			onGround_ = true;
		
		}

	}




	
	//移動
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;
	
	//左右イージング
	if (turnTimer_ >0.0f) {
		turnTimer_ -= 1.0f / 60.0f;
	
		float destinationRotationYTable[] = {
			std::numbers::pi_v<float> / 2.0f,
			std::numbers::pi_v<float> * 3.0f / 2.0f,

		};

		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		worldTransform_.rotation_.y = EaseInSine(turnFirstRotationY_, destinationRotationY, turnTimer_, kTimeTurn) ;
	}



	//アセット処理?
	MakeAffineMatrix(&worldTransform_);

}

void Player::Draw() {

	model_->Draw(worldTransform_, *camera_);
	
}
