#include "GameScene.h"
#include "2d/ImGuiManager.h"
#include <cassert>
#include "function.h"


using namespace KamataEngine;

GameScene::GameScene() { 

	Initialize();

}

GameScene::~GameScene() { 
	delete sprite_; 
	delete model_;
	delete debugCamera_;
	delete player_;
	delete deathParticles_;
	//delete enemy_;
	delete modelBlock_;
	delete modelSkydome_;
	delete modelEnemy_;
	delete modelEnemyDeathEffect_;
	delete modelAttack_;
	delete modelDeathParticles_;
	delete skydome_;
	delete mapChipField_;
	delete cameraController;
	delete fade_;


	for (Enemy* enemy : enemies_) {
		delete enemy;
	}

	for (HitEffect* enemyDeathParticles_ : enemyDeathParticless_) {
		delete enemyDeathParticles_;
	}

	enemies_.clear();

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlocks : worldTransformBlockLine) {	
			delete worldTransformBlocks;
		}
	}


	worldTransformBlocks_.clear();
}


void GameScene::Initialize() {
	textureHandle_ = TextureManager::Load("matyacap.png");
	//soundDataHandle_ = Audio::GetInstance()->LoadWave("mokugyo.wav");

	sprite_ = Sprite::Create(textureHandle_, {100, 50});
	model_ = Model::CreateFromOBJ("player", true);
	modelBlock_ = Model::CreateFromOBJ("block", true);
	modelSkydome_ = Model::CreateFromOBJ("skydome",true);
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);
	modelDeathParticles_ = Model::CreateFromOBJ("deathParticle", true);
	modelAttack_ = Model::CreateFromOBJ("hit_effect", true);
	modelEnemyDeathEffect_ = Model::CreateFromOBJ("particle", true);

	worldTransform_.Initialize();
	camera_.farZ = 1500.0f;
	camera_.Initialize();

	//Audio::GetInstance()->PlayWave(soundDataHandle_);
	//voiceHandle_ = Audio::GetInstance()->PlayWave(soundDataHandle_, true);

	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	debugCamera_ = new DebugCamera(1280,720);
	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());
	


	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	const uint32_t kNumBlockVirtical = 10;
	const uint32_t kNumBlockHorizontal = 20;
	//const float kBlockWidth = 2.0f;
	//const float kBlockHeight = 2.0f;

	worldTransformBlocks_.resize(kNumBlockVirtical);

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	/* for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if (i % 2 == 0) {
				worldTransformBlocks_[i][j] = nullptr;
				continue;
			}

			if (j % 2 == 0) {
				worldTransformBlocks_[i][j] = nullptr;
				continue;
			}


			worldTransformBlocks_[i][j] = new WorldTransform();
			worldTransformBlocks_[i][j]->Initialize();
			worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
			worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;

		}
	}*/



	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/block.csv");

	GenerateBlocks();
	KamataEngine::Vector3 playerPos = mapChipField_->GetMapChipPositionByIndex(5,5); 
	
	player_ = new Player();
	player_->Initialize(model_, modelAttack_, &camera_, playerPos);
	player_->SetMapChipField(mapChipField_);

	deathParticles_ = new DeathParticles();
	deathParticles_->Initialize(modelDeathParticles_, &camera_, playerPos);

	//KamataEngine::Vector3 enemyPos = mapChipField_->GetMapChipPositionByIndex(5,5); 
	//enemy_ = new Enemy();
	//enemy_->Initialize(modelEnemy_, &camera_, enemyPos);

	for (int32_t i = 0; i < kMaxEnemy; i++) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPos = mapChipField_->GetMapChipPositionByIndex(20 * i, 5); 
		newEnemy->Initialize(modelEnemy_, &camera_, enemyPos);
		newEnemy->SetGameScene(this);
		enemies_.push_back(newEnemy);
		
	}




	cameraController = new CameraController();

	CameraController::Rect area;
	area.left = 0.0f;
	area.right = float(mapChipField_->GetNumBlockHorizontal());
	area.bottom = 0.0f;
	area.top = float(mapChipField_->GetNumBlockVirtical());


	cameraController->SetMovebleArea(area);

	

	cameraController->Initialize(&camera_);
	cameraController->SetTarget(player_);
	cameraController->Reset();

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);


	for (int32_t i = 0; i < kMaxEnemy; i++) {
		HitEffect* newEnemyDeathParticles = new HitEffect();
		Vector3 enemyDeathParticlesPos = mapChipField_->GetMapChipPositionByIndex(20 * i, 5);
		newEnemyDeathParticles->Initialize(enemyDeathParticlesPos);
		newEnemyDeathParticles->SetCamera(&camera_);
		newEnemyDeathParticles->SetModel(modelEnemyDeathEffect_);
		
		enemyDeathParticless_.push_back(newEnemyDeathParticles);
	}

	InitializeRandom();

}

void GameScene::Update() {

	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->GetIsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});


	PhaseChange();
	fade_->Update();

	#ifdef _DEBUG

	//if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
	//	Audio::GetInstance()->StopWave(voiceHandle_);
	//}
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = true;
	}


	ImGui::Begin("Debug1");
	ImGui::Text("Kamata Tarou %d %d %d", 2050, 12, 31);
	ImGui::End();

		ImGui::Begin("Debug2");
	ImGui::InputFloat3("InputFloat3", inputFloat3);
	ImGui::SliderFloat3("InputFloat3", inputFloat3,0.0f,1.0f);

	ImGui::End();

	ImGui::ShowDemoWindow();

	#endif

}

void GameScene::Draw() {

	Sprite::PreDraw();

	//sprite_->Draw();



	Sprite::PostDraw();

	
	Model::PreDraw();

	//model_->Draw(worldTransform_, debugCamera_->GetCamera());
	if (!player_->GetIsDead()) {
		player_->Draw();
	}

	skydome_->Draw();
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

		
	for (HitEffect* enemyDeathParticles : enemyDeathParticless_) {
		
		enemyDeathParticles->Draw();
	}
	//enemy_->Draw();

	// デスパーティクルが存在するなら...???
	if (deathParticles_) {
		deathParticles_->Draw();
	}
	
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {

		for (WorldTransform* worldTransformBlocks : worldTransformBlockLine) {
			if (!worldTransformBlocks) {
				continue;
			}

			modelBlock_->Draw(*worldTransformBlocks, camera_);
		}
	
	}

	PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});
			


	Model::PostDraw();

	fade_->Draw();
}

void GameScene::GenerateBlocks() {

	uint32_t numBlockVirtcal = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numBlockVirtcal);

	for (uint32_t i = 0; i < numBlockVirtcal; i++) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	
	}

	for (uint32_t y = 0; y < numBlockVirtcal; y++) {
		for (uint32_t x = 0; x < numBlockHorizontal; x++) {
			if (mapChipField_->GetMapChipTypeByIndex(x,y) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[y][x] = worldTransform;
				worldTransformBlocks_[y][x]->translation_ = mapChipField_->GetMapChipPositionByIndex(x,y);
			
			
			}

		}
	
	}


}

void GameScene::PhaseChange() {

	switch (phase_) {
	case GameScene::Phase::kFadeIn:
		GamePlayPhase();

		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
			fade_->Stop();
		}
		
		break;

	case GameScene::Phase::kPlay:
		GamePlayPhase();

		if (player_->GetIsDead()) {
			phase_ = Phase::kDeath;
			const Vector3& deathParticlesPos = player_->GetWorldPos();
			deathParticles_->Initialize(modelDeathParticles_, &camera_, deathParticlesPos);
		}

		break;

	case GameScene::Phase::kDeath:
		DeathParticlePhase();
		if (deathParticles_->GetIsFinished()) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		

		break;

	case GameScene::Phase::kFadeOut:

		if (fade_->IsFinished()) {
			if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
				finished_ = true;
			}
		}

		break;
	}
}

void GameScene::GamePlayPhase() {
	// 天球の更新
	skydome_->Update();

	// 自キャラの更新
	player_->Update();

	// 敵の更新(複数)
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}
	for (HitEffect* enemyDeathParticles : enemyDeathParticless_) {
		enemyDeathParticles->Update();
	}

	// カメラコントロールの更新
	cameraController->Update();

	// カメラの更新
	CameraUpdate();

	// ブロックの更新
	BlocksUpdate();

	// 全ての当たり判定
	CheckAllCollisions();

}

void GameScene::DeathParticlePhase() {
	// 天球の更新
	skydome_->Update();

	// 敵の更新(複数)
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	// デスパーティクルが存在するなら...???
	if (deathParticles_) {
		deathParticles_->Update();
	}

	// カメラの更新
	CameraUpdate();

	// ブロックの更新
	BlocksUpdate();
}

void GameScene::CameraUpdate() {

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
	} else {
		camera_.UpdateMatrix();
	}

}

void GameScene::BlocksUpdate() {
	
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlocks : worldTransformBlockLine) {
			if (!worldTransformBlocks) {
				continue;
			}
			MakeAffineMatrix(worldTransformBlocks);
		}
	}

}

void GameScene::CheckAllCollisions() {

	#pragma region 自キャラと敵キャラ当たり判定
	{
		AABB aabb1, aabb2;
		aabb1 = player_->GetAABB();
		for (Enemy* enemy : enemies_) {

			if (enemy->GetIsCollisionDisabled()) {
				continue;
			}

			aabb2 = enemy->GetAABB();

			if (IsCollisionAABB2D(aabb1,aabb2)) {
				player_->OnCollision(enemy);
				enemy->OnCollision(player_);

			}
			
		}

	}
	#pragma endregion



}

void GameScene::CreateHitEffect(KamataEngine::Vector3 pos) { 
	HitEffect* newEnemyDeathParticles = HitEffect::Create(pos);
	newEnemyDeathParticles->SetIsDead();
	enemyDeathParticless_.push_back(newEnemyDeathParticles);

}

