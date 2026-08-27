#pragma once
#include "KamataEngine.h"
#include "function.h"

class Player;
class GameScene;
class MapChipField;

class EnemyMini {
public:

	enum class Behavior {
		kMove,
		kDeath,
		kTurn,
		kUnknown,
	};

public:

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3 pos);

	void Update();

	void Draw();

	KamataEngine::Vector3 GetWorldPos();
	AABB GetAABB();

	void OnCollision();
	void OnAttackCollision();

	void BehaviorMoveInitialize();
	void BehaviorTurnInitialize();
	void BehaviorDeathInitialize();

	bool ShouldTurn();

	bool GetIsDead() const { return isDead_; };
	bool GetIsCollisionDisabled() const { return isCollisionDisabled_; };
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; };

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; };

	void SetPlayer(Player* player) { player_ = player; }

private:
	//初期設定
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	uint32_t textureHandle_ = 0u;

	//移動
	KamataEngine::Vector3 velocity_ = {};
	static inline const float kWalkSpeed = 0.05f;

	//アニメーション
	static inline const float kWalkMotionAngleStart = 10.0f;
	static inline const float kWalkMotionAngleEnd = -10.0f;
	static inline const float kWalkMotionTime = 1.0f;

	float walkTimer_ = 0.0f;

	float startRot = 0.0f;
	float targetRot = 0.0f;

	uint32_t turnParameter_ = 0;
	uint32_t turnTime = 60;

	// 当たり判定

	static inline const float kWidth = 0.8f;
	static inline const float kHeigth = 0.8f;

	//死亡
	bool isDead_ = false;

	//死亡演出
	Behavior behavior_ = Behavior::kMove;
	Behavior behaviorRequest_ = Behavior::kMove;
	uint32_t deathParameter_ = 0;
	uint32_t deathTime = 60;

	bool isCollisionDisabled_ = false;

	GameScene * gameScene_ = nullptr;

	//その他
	MapChipField* mapChipField_ = nullptr;

	Player* player_ = nullptr;

	// 発見距離
	static inline const float kRushRange = 8.0f;

	// 通常より速い速度
	static inline const float kRushSpeed = 0.15f;

};
