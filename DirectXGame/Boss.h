#pragma once
#include "KamataEngine.h"
#include "function.h"
#include <numbers> 
#include <cassert> 
#include <vector>

class Player;
class GameScene;

class Boss {
public:



	enum class Behavior {
		kMove,
		kDeath,
		kDamageReaction,
		kKnockback, 
		kUnknown,
	};

	enum class KnockbackStatus {
		kBackward,
		kForward,
	};

	enum class AttackPhase {

		kWait,
		kRise,
		kFall,

	};


public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Model* attackModel, KamataEngine::Camera* camera, const KamataEngine::Vector3 pos);

	void Update();
	void KnockbackUpdata();
	void AttackUpdata();
	void ShockWaveUpdata();



	void Draw();


	KamataEngine::Vector3 GetWorldPos();
	AABB GetAABB();

	void OnCollision( Player* player);
	void OnAttackCollision(uint32_t attackID);

	void BehaviorMoveInitialize();
	void BehaviorkDamageReactionInitialize();
	void BehaviorKnockbackInitialize();
	void BehaviorDeathInitialize();

	void AddSwingImpact();

	bool GetIsDead() const { return isDead_; };
	bool GetIsCollisionDisabled() const { return isCollisionDisabled_; };
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; };
	void isBehaviorRequestKnockback() { isKnockbackRequest = true; }


	void SetIsClearBoss(bool flag) { isClearBoss_ = flag; }

	bool GetIsClearBoss() const { return isClearBoss_; }

	void SetAttackModel(KamataEngine::Model* model) { attackModel_ = model; }

	bool IsShockWaveActive() const { return isShockWaveActive_; }

	AABB GetShockWaveAABB();

	void DisableShockWave() { isShockWaveActive_ = false; }

private:

	LRDirection lrDirection_ = LRDirection::kLeft;

	// 初期設定
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	uint32_t textureHandle_ = 0u;

	// 移動
	KamataEngine::Vector3 velocity_ = {};
	static inline const float kWalkSpeed = 0.05f;

	// アニメーション
	static inline const float kWalkMotionAngleStart = -30.0f;
	static inline const float kWalkMotionAngleEnd = -150.0f;
	static inline const float kWalkMotionTime = 1.0f;
	static inline const float kKnockbackMotionTime = 15.0f;


	float walkTimer_ = 0.0f;

	KnockbackStatus knockbackStatus = KnockbackStatus::kBackward;

	float knockbackTimer_ = 0.0f;
	bool isKnockbackRequest;

	//揺れ
	float swingAngle_ = 0.0f;
	float swingVelocity_ = 0.0f;

	 float kSwingSpring = 0.004f;
	 float kSwingDamping = 0.995f;
	 float kHitSwingPower = 0.04f;
	static inline const float kDeathSwingAngle = ((std::numbers::pi_v<float> / 2.0f) / 10.0f) * 8.0f;
	static inline const float kDeathSwingAngleA = ((std::numbers::pi_v<float> / 2.0f) / 10.0f);

	uint32_t damageReactionParameter_ = 0;
	float damageReactionStartAngle_ = 0.0f;

	static inline const uint32_t kDamageReactionTime = 120;
	static inline const float kDamageReactionRotateCount = 2.0f;

	float damageReactionStartY_ = 0.0f;
	static inline const float kDamageReactionFaceRotateCount = 2.0f;

	// 当たり判定

	static inline const float kWidth = 0.95f;
	static inline const float kHeigth = 1.15f;
	
	// 当たり判定中心を上にずらす
	static inline const float kCollisionOffsetY = 1.2f;

	std::vector<uint32_t> hitAttackIDs_;

	//ライフ
	uint32_t life = 3;

	// 死亡
	bool isDead_ = false;

	// 死亡演出
	Behavior behavior_ = Behavior::kMove;
	Behavior behaviorRequest_ = Behavior::kMove;
	uint32_t deathParameter_ = 0;
	uint32_t deathTime = 60;

	bool isCollisionDisabled_ = false;

	GameScene* gameScene_ = nullptr;

	// ========================================
	// 最終ボス戦用
	// ========================================

	// 倒すとクリア条件に含まれるBossか
	bool isClearBoss_ = false;


	AttackPhase attackPhase_ = AttackPhase::kWait;

	// 攻撃間隔
	uint32_t attackTimer_ = 0;

	// 8秒待機
	static inline const uint32_t kAttackInterval = 480;

	// 元いた高さ
	float groundY_ = 0.0f;

	// 上昇・下降時間
	uint32_t jumpTimer_ = 0;

	static inline const uint32_t kRiseTime = 40;
	static inline const uint32_t kFallTime = 10;

	static inline const float kJumpHeight = 6.0f;

	// 衝撃波
	KamataEngine::Model* attackModel_ = nullptr;

	KamataEngine::WorldTransform shockWaveTransform_;

	bool isShockWaveActive_ = false;

	uint32_t shockWaveTimer_ = 0;

	static inline const uint32_t kShockWaveLifeTime = 90;

	static inline const float kShockWaveSpeed = 0.35f;

	static inline const float kShockWaveWidth = 0.8f;
	static inline const float kShockWaveHeight = 0.8f;


};
