#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "function.h"
#include <numbers> 
#include <cassert> 
#include "ChargeEffect.h"

class Enemy;
class ShieldEnemy;

class Player {


private:

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


	enum class Behavior {
		kRoot, 
		kDash, 
		kKnockback,
		kUnknown,
	};

	enum class DashPhase { 
		kCharge,
		kRush,
		kLingering,
	};


	enum class ArmState {
		kIdle,
		kCharge,
		kAttack,
		kChargeAttack,
		kReturn,

	};

	enum class ChargeAttackPhase {
		kPrepare,
		kFall,
		kImpact,
	};

	struct ChargeAttackArmConfig {
		KamataEngine::Vector3 preparePos;
		KamataEngine::Vector3 fallPos;
		float startRot;
		const float prepareRot = (std::numbers::pi_v<float> / 2.0f) * 4.0f;
		const uint32_t kPrepareTime = 30;
		const uint32_t kFallTime = 10;
		const uint32_t kImpactTime = 45;

		ChargeAttackPhase chargeAttackPhase = ChargeAttackPhase::kPrepare;
		;

	};



public:
	Player();
	~Player();
	void Initialize(
	    KamataEngine::Model* model, KamataEngine::Model* modelArm, KamataEngine::Model* modelTire, KamataEngine::Model* modelAttack, KamataEngine::Model* modelChargeEffect,
	    KamataEngine::Camera* camera, const KamataEngine::Vector3 pos);

	void Update();

	void Draw();


	void MovementInput();
	void MapCollisionDetection(CollisionMaPInfo& info);
	void JudgmentMovement(const CollisionMaPInfo& info);
	void CeilingContactDetection(const CollisionMaPInfo& info);
	void WallDetection(const CollisionMaPInfo& info);
	void GroundStateSwitching(const CollisionMaPInfo& info);

	void TurningControl();

	//地面天井壁当たり判定
	void MCDUpDirection(CollisionMaPInfo& info);
	void MCDDownDirection(CollisionMaPInfo& info);
	void MCDRightDirection(CollisionMaPInfo& info);
	void MCDLeftDirection(CollisionMaPInfo& info);

	//プレイヤーの状態初期化
	void BehaviorRootInitialize();
	void BehaviorDashInitialize();
	void BehaviorKnockbackInitialize();
	void AttackInitialize();

	// プレイヤーの状態
	void BehaviorRootUpdate();
	void BehaviorDashUpdate();
	void BehaviorKnockbackUpdate();

	//攻撃
	void AttackUpdate();
	void RightArmAttackUpdate();
	void LeftArmAttackUpdate();

	void NormalAttackInitialize(KamataEngine::WorldTransform& armTransform, KamataEngine::Vector3& startPos, KamataEngine::Vector3& targetPos, uint32_t& parameter);
	void NormalAttackMove(KamataEngine::WorldTransform& armTransform, const KamataEngine::Vector3& startPos, const KamataEngine::Vector3& targetPos, KamataEngine::Vector3& returnStartPos, uint32_t& parameter,ArmState& state);

	void ChargeAttackInitialize(KamataEngine::WorldTransform& armTransform, ChargeAttackArmConfig& CAAC, KamataEngine::Vector3& startPos, KamataEngine::Vector3& returnStartPos ,uint32_t& parameter);
	void ChargeAttackMove( KamataEngine::WorldTransform& armTransform, ChargeAttackArmConfig& CAAC, KamataEngine::Vector3& startPos, KamataEngine::Vector3& returnStartPos, uint32_t& parameter, ArmState& state);


	void ArmReturnInitialize(KamataEngine::WorldTransform& armTransform, KamataEngine::Vector3& startPos, uint32_t& parameter);
	void ArmReturnMove(KamataEngine::WorldTransform& armTransform, const KamataEngine::Vector3& startPos, const KamataEngine::Vector3& targetPos, uint32_t& parameter, ArmState& state);


	void armUpdata();

	void ChargeShotUpdate();

	void TireUpdate(float moveX);

	KamataEngine::Vector3 GetRightArmPos();
	KamataEngine::Vector3 GetLeftArmPos();
	
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	const KamataEngine::WorldTransform& GetWorldTransform();
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }
	KamataEngine::Vector3 GetWorldPos() ;
	AABB GetAABB();
	AABB GetRightArmAttackAABB();
	AABB GetLeftArmAttackAABB();

	AABB GetRightChargeShotAABB();
	AABB GetLeftChargeShotAABB();

	// 今その判定を有効にしていいか
	bool IsRightArmAttacking() const;
	bool IsLeftArmAttacking() const;

	bool IsRightChargeShotActive() const { return isRightChargeShot; }
	bool IsLeftChargeShotActive() const { return isLeftChargeShot; }
	uint32_t GetRightArmAttackID() const { return rightArmAttackID_; }

	uint32_t GetLeftArmAttackID() const { return leftArmAttackID_; }

	uint32_t GetRightChargeShotAttackID() const { return rightChargeShotAttackID_; }

	uint32_t GetLeftChargeShotAttackID() const { return leftChargeShotAttackID_; }

	void DisableRightChargeShot();
	void DisableLeftChargeShot();


	bool GetIsDead() const { return isDead_; };

	void SetMapChipField(MapChipField* mapChipField) ;
	void OnCollision(const Enemy* enemy);
	void OnShieldCollision(const ShieldEnemy* shieldEnemy);

	void OnEnemyCollision();

	void VariousMakeAffineMatrix();
	
	bool isDash() const {
		if (behavior_ == Behavior::kDash) {
			return true;
		}

		return false;
	}

	void isBehaviorRequestKnockback() { 
		isKnockbackRequest = true;
	
	}

	const KamataEngine::Vector3& GetWorldTransform() const { return worldTransform_.translation_; };
	
	const LRDirection GetLRDirection() const { return lrDirection_; };
	void MakeTireAffineMatrix(KamataEngine::WorldTransform* worldTransform);


	void Debug();

private:

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;

	KamataEngine::WorldTransform worldTransformRightArm_;
	KamataEngine::WorldTransform worldTransformLeftArm_;
	KamataEngine::Model* modelArm_ = nullptr;

	KamataEngine::WorldTransform worldTransformTire_;
	KamataEngine::Model* modelTire_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;
	uint32_t textureHandle_ = 0u;

	//左右移動
	KamataEngine::Vector3 velocity_ = {};
	static inline const float kAcceleration = 0.05f;
	static inline const float kLimitRunSpeed = 0.3f;

	//左右向き
	LRDirection lrDirection_ = LRDirection::kRight;
	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;
	static inline const float kTimeTurn = 0.3f;

	//ジャンプ
	bool onGround_ = true;
	static inline const float kGravityAcceleration = 0.05f;
	static inline const float kLimitFallSpeed = 0.5f;
	static inline const float kLimitJumpSpeed = 1.0f;
	static inline const float kJumpAcceleration = 1.0f;
	static inline const float kAttenuation = 1.0f;	
	static inline const float kAttenuationLanding =	0.5f;	
	static inline const float kAttenuationWall = 0.5f;	

	//当たり判定
	MapChipField* mapChipField_ = nullptr;

	static inline const float kWidth = 0.8f;
	static inline const float kHeigth = 0.8f;
	static inline const float kBlank = 0.05f;

	//死亡
	bool isDead_ = false;

	//ステート
	Behavior behavior_ = Behavior::kRoot;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	//ダッシュ
	DashPhase dashPhase_ = DashPhase::kCharge;

	uint32_t dashParameter_ = 0;
	uint32_t dashTime = 15;
	uint32_t chargeTime = 5; 
	uint32_t rushTime = 5; 
	uint32_t lingeringTime = 5; 

	KamataEngine::Vector3 dashVelocity = {1.0f,0.0f,0.0f};

	//攻撃
	uint32_t attackParameter_ = 0;
	uint32_t attackTime = 10;

	bool preAttackKey_ = false;
	uint32_t attackHoldFrame_ = 0;
	static inline const uint32_t kPunchAttackFrame = 45;
	static inline const uint32_t kChargeAttackFrame = 135;



	KamataEngine::Vector3 rightArmAttackStartPos_;
	KamataEngine::Vector3 rightArmAttackTargetPos_;

	KamataEngine::Vector3 leftArmAttackStartPos_;
	KamataEngine::Vector3 leftArmAttackTargetPos_;

	KamataEngine::Vector3 rightArmReturnStartPos_;
	KamataEngine::Vector3 leftArmReturnStartPos_;

	ChargeAttackArmConfig rightArmCAAC;
	ChargeAttackArmConfig leftArmCAAC;

	uint32_t rightArmParameter_ = 0;
	uint32_t leftArmParameter_ = 0;

	float kArmAttackDistance = 10.0f;
	uint32_t kArmAttackTime = 60;
	uint32_t kArmReturnTime = 180;

	// 攻撃ID
	uint32_t attackIDCounter_ = 0;

	uint32_t rightArmAttackID_ = 0;
	uint32_t leftArmAttackID_ = 0;

	uint32_t rightChargeShotAttackID_ = 0;
	uint32_t leftChargeShotAttackID_ = 0;

	//攻撃エフェクト
	KamataEngine::Model* modelAttack_ = nullptr;
	KamataEngine::WorldTransform worldTransformDash_;	
	ChargeEffect* chargeEffect_ = nullptr;

	KamataEngine::WorldTransform worldTransformLeftChargeShot_;	
	KamataEngine::WorldTransform worldTransformRightChargeShot_;	
	bool isLeftChargeShot;
	bool isRightChargeShot;
	LRDirection lRDLeftChargeShot;
	LRDirection lRDRightChargeShot;
	uint32_t leftChargeShotParameter_ = 0;
	uint32_t rightChargeShotParameter_ = 0;
	uint32_t leftChargeShotTime = 90;
	uint32_t rightChargeShotTime = 90;


	//ノックバック　
	bool isKnockbackRequest = false;
	uint32_t knockbackParameter_ = 0;

	//部品パーツ
	ArmState armRightState = ArmState::kIdle;
	ArmState armLeftState = ArmState::kIdle;

	static inline const float kArmOffsetY = 0.25f;
	static inline const float kDashArmBackOffset = 0.5f;

	float tireRotation_ = 0.0f;
	static inline const float kTireOffsetY = -0.25f;

	// 回転速度
	static inline const float kTireRotationSpeed = 3.0f;

	// 空中移動
	static inline const float kAirAcceleration = 0.02f;
	static inline const float kAirLimitRunSpeed = 0.35f;
	static inline const float kAirDeceleration = 0.08f;

};
