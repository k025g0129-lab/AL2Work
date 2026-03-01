#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "function.h"

class Enemy;

class Player {


private:
	enum class LRDirection {
		kRight,
		kLeft,
	};

	struct CollisionMaPInfo{
		bool isCeilingCollision = false;
		bool isLanding = false;
		bool isWallContact = false;
		KamataEngine::Vector3 moveAmount; 
	
	};

	enum Corner {
		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,

		kNumCorner
	};

public:
	Player();
	~Player();
	void Initialize(KamataEngine::Model* model,KamataEngine::Camera * camera,const KamataEngine::Vector3 pos);

	void Update();

	void Draw();


	void MovementInput();
	void MapCollisionDetection(CollisionMaPInfo& info);
	void JudgmentMovement(const CollisionMaPInfo& info);
	void CeilingContactDetection(const CollisionMaPInfo& info);
	void WallDetection(const CollisionMaPInfo& info);
	void GroundStateSwitching(const CollisionMaPInfo& info);

	void TurningControl();

	void MCDUpDirection(CollisionMaPInfo& info);
	void MCDDownDirection(CollisionMaPInfo& info);
	void MCDRightDirection(CollisionMaPInfo& info);
	void MCDLeftDirection(CollisionMaPInfo& info);



	
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	const KamataEngine::WorldTransform& GetWorldTransform();
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }
	KamataEngine::Vector3 GetWorldPos() ;
	AABB GetAABB();

	void SetMapChipField(MapChipField* mapChipField) ;
	void OnCollision(const Enemy* enemy);



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
	static inline const float kGravityAcceleration = 0.05f;
	static inline const float kLimitFallSpeed = 0.5f;
	static inline const float kJumpAcceleration = 1.0f;
	static inline const float kAttenuation = 1.0f;	
	static inline const float kAttenuationLanding =	0.5f;	
	static inline const float kAttenuationWall = 0.5f;	

	//当たり判定
	MapChipField* mapChipField_ = nullptr;

	static inline const float kWidth = 0.8f;
	static inline const float kHeigth = 0.8f;
	static inline const float kBlank = 0.05f;

};
