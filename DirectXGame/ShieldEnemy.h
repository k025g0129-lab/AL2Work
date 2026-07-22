#pragma once
#include "KamataEngine.h"
#include "function.h"

class Player;
class GameScene;

class ShieldEnemy {
public:



	enum class Behavior {
		kMove,
		kDeath,
		kKnockback, 
		kUnknown,
	};

	enum class KnockbackStatus {
		kBackward,
		kForward,
	};


public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3 pos);

	void Update();
	void KnockbackUpdata();

	void Draw();

	KamataEngine::Vector3 GetWorldPos();
	AABB GetAABB();

	void OnCollision( Player* player);

	void BehaviorMoveInitialize();
	void BehaviorKnockbackInitialize();
	void BehaviorDeathInitialize();


	bool GetIsDead() const { return isDead_; };
	bool GetIsCollisionDisabled() const { return isCollisionDisabled_; };
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; };
	void isBehaviorRequestKnockback() { isKnockbackRequest = true; }

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
	// 当たり判定

	static inline const float kWidth = 0.8f;
	static inline const float kHeigth = 0.8f;

	// 死亡
	bool isDead_ = false;

	// 死亡演出
	Behavior behavior_ = Behavior::kMove;
	Behavior behaviorRequest_ = Behavior::kMove;
	uint32_t deathParameter_ = 0;
	uint32_t deathTime = 60;

	bool isCollisionDisabled_ = false;

	GameScene* gameScene_ = nullptr;

};
