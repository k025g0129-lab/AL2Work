#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "Skydome.h"
#include <vector>
#include "MapChipField.h"
#include "CameraController.h"
#include "Enemy.h"
#include "DeathParticles.h"
#include "Fade.h"

class GameScene{
public:

	KamataEngine::Sprite* sprite_ = nullptr;

	enum class Phase {
		kFadeIn,
		kPlay,
		kDeath,
		kFadeOut,
	};
	
public:

	GameScene();
	~GameScene();



	void Initialize();
	void Update();
	void Draw();

	void GenerateBlocks();

	//フェーズ関数
	void PhaseChange();

	void GamePlayPhase();
	void DeathParticlePhase();

	//フェーズ内部処理関数
	void CameraUpdate();
	void BlocksUpdate();
	void CheckAllCollisions();

	bool GetIsFinished() const { return finished_; };

private:
	uint32_t textureHandle_ = 0;
	uint32_t soundDataHandle_ = 0;
	uint32_t voiceHandle_ = 0;


	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;
	KamataEngine::Model* modelEnemy_	 = nullptr;
	KamataEngine::Model* modelDeathParticles_ = nullptr;
	KamataEngine::Model* modelAttack_ = nullptr;

	KamataEngine::WorldTransform worldTransform_;
	std::vector < std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	KamataEngine::Camera camera_;

	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	Player* player_ = nullptr;
	DeathParticles* deathParticles_ = nullptr;

	std::list<Enemy*> enemies_;
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

};
