#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "Skydome.h"
#include <vector>
#include "MapChipField.h"
#include "CameraController.h"
#include "Enemy.h"
#include "EnemyMini.h"
#include "Boss.h"
#include "DeathParticles.h"
#include "Fade.h"
#include "HitEffect.h"
#include "ShieldEnemy.h"
#include "GuardEffect.h"
#include "StageManager.h"


class GameScene{
public:

	KamataEngine::Sprite* sprite_ = nullptr;

	enum class Phase {
		kFadeIn,
		kPlay,
		kDeath,
		kClear,
		kFadeOut,
	};
	
public:

	GameScene(StageManager* stageManager);
	~GameScene();


		
	void Initialize(StageManager* stageManager);
	void Update();
	void Draw();

	void GenerateField();

	void CreateEnemy(const uint32_t& x, const uint32_t& y);
	void CreateShieldEnemy(const uint32_t& x, const uint32_t& y);
	void CreateEnemyMini(const uint32_t& x, const uint32_t& y);
	void CreateBoss(const uint32_t& x, const uint32_t& y, bool isClearBoss);

	//フェーズ関数
	void PhaseChange();

	void GamePlayPhase();
	void DeathParticlePhase();

	//フェーズ内部処理関数
	void CameraUpdate();
	void BlocksUpdate();
	void CheckAllCollisions();

	void CreateHitEffect(KamataEngine::Vector3 pos);
	void CreateGuardEffect(KamataEngine::Vector3 pos);

	bool GetIsFinished() const { return finished_; };
	bool GetIsReloadRequseted() const { return reloadRequseted; };

	void CreateEnemyMini(const KamataEngine::Vector3& pos);

private:
	uint32_t textureHandle_ = 0;
	uint32_t soundDataHandle_ = 0;	
	uint32_t voiceHandle_ = 0;


	KamataEngine::Model* modelBody_ = nullptr;
	KamataEngine::Model* modelArm_ = nullptr;
	KamataEngine::Model* modelTire_ = nullptr;
	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;
	KamataEngine::Model* modelEnemy_	 = nullptr;
	KamataEngine::Model* modelEnemyMini_	 = nullptr;
	KamataEngine::Model* modelBoss_	 = nullptr;
	KamataEngine::Model* modelShieldEnemy_ = nullptr;
	KamataEngine::Model* modelDeathParticles_ = nullptr;
	KamataEngine::Model* modelPlayerAttackEffect_ = nullptr;
	KamataEngine::Model* modelEnemyAttackEffect_ = nullptr;
	KamataEngine::Model* modelEnemyDeathEffect_ = nullptr;
	KamataEngine::Model* modelEnemyGuardEffect_ = nullptr;


	KamataEngine::WorldTransform worldTransform_;
	std::vector < std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	KamataEngine::Camera camera_;

	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	Player* player_ = nullptr;
	DeathParticles* deathParticles_ = nullptr;

	std::list<Enemy*> enemies_;
	std::list<ShieldEnemy*> shieldEnemies_;
	std::list<EnemyMini*> miniEnemies_;
	std::list<Boss*> bosss_;

	//Enemy* enemy_ = nullptr;
	Skydome* skydome_ = nullptr;

	int32_t kMaxEnemy = 5;

	float inputFloat3[3] = {0.0f, 0.0f, 0.0f};

	bool isDebugCameraActive_ = false;


	MapChipField* mapChipField_;	

	CameraController* cameraController;

	Fade* fade_ = nullptr;	
	Phase phase_= Phase::kPlay;


	bool finished_ = false;

	bool reloadRequseted = false;


	//HitEffect* enemyDeathParticles_ = nullptr;
	std::list<HitEffect*> enemyDeathParticless_;
	std::list<GuardEffect*> enemyGuardParticless_;

	StageManager* stageManager_ = nullptr;

	//1supe-suki- 
	uint32_t pressSpaceTextureHandle_ = 0;
	KamataEngine::Sprite* pressSpaceSprite_ = nullptr;

	// ゲームオーバーUI
	uint32_t gameOverTextureHandle_ = 0;
	KamataEngine::Sprite* gameOverSprite_ = nullptr;
	// クリアUI
	uint32_t clearTextureHandle_ = 0;
	KamataEngine::Sprite* clearSprite_ = nullptr;

	uint32_t resultTimer_ = 0;

	bool isClearBossDefeated_ = false;
};
