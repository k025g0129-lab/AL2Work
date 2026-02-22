#pragma once
#include "KamataEngine.h"

class Player {
public:
	Player();
	~Player();
	void Initialize(KamataEngine::Model* model,KamataEngine::Camera * camera,const KamataEngine::Vector3 pos);

	void Update();

	void Draw();


	const KamataEngine::WorldTransform& GetWorldTransform();
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

private:
	enum class LRDirection {
		kRight,
		kLeft,
	};

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	uint32_t textureHandle_ = 0u;

	//左右移動
	KamataEngine::Vector3 velocity_ = {};
	static inline const float kAcceleration = 0.1f;
	static inline const float kLimitRunSpeed = 1.0f;

	//左右向き
	LRDirection lrDirection_ = LRDirection::kRight;
	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;
	static inline const float kTimeTurn = 0.3f;

	//ジャンプ
	bool onGround_ = true;
	static inline const float kGravityAcceleration = 0.1f;
	static inline const float kLimitFallSpeed = 1.0f;
	static inline const float kJumpAcceleration = 0.5f;
	static inline const float kAttenuation = 1.0f;	


};
