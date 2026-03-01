#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "Skydome.h"
#include <vector>
#include "MapChipField.h"
#include "CameraController.h"
#include "Enemy.h"


class GameScene{
public:

	KamataEngine::Sprite* sprite_ = nullptr;
	
public:

	GameScene();
	~GameScene();



	void Initialize();
	void Update();
	void Draw();

	void GenerateBlocks();

	// 全ての当たり判定
	void CheckAllCollisions();



private:
	uint32_t textureHandle_ = 0;
	uint32_t soundDataHandle_ = 0;
	uint32_t voiceHandle_ = 0;


	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;
	KamataEngine::Model* modelEnemy_	 = nullptr;

	KamataEngine::WorldTransform worldTransform_;
	std::vector < std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	KamataEngine::Camera camera_;

	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	Player* player_ = nullptr;
	std::list<Enemy*> enemies_;
	//Enemy* enemy_ = nullptr;
	Skydome* skydome_ = nullptr;

	int32_t kMaxEnemy = 5;

	float inputFloat3[3] = {0.0f, 0.0f, 0.0f};

	bool isDebugCameraActive_ = false;


	MapChipField* mapChipField_;	

	CameraController* cameraController;
};
