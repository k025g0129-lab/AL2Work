#include "GameScene.h"
#include "2d/ImGuiManager.h"
#include <cassert>
#include "function.h"


using namespace KamataEngine;

GameScene::GameScene(StageManager* stageManager) { 

	Initialize(stageManager);

}

GameScene::~GameScene() { 
	delete sprite_; 
	delete modelBody_;
	delete modelArm_;
	delete modelTire_;
	delete debugCamera_;
	delete player_;
	delete deathParticles_;
	//delete enemy_;
	delete modelBlock_;
	delete modelSkydome_;
	delete modelEnemy_;
	delete modelEnemyMini_;
	delete modelBoss_;
	delete modelShieldEnemy_;
	delete modelEnemyDeathEffect_;
	delete modelEnemyGuardEffect_;
	delete modelPlayerAttackEffect_;
	delete modelEnemyAttackEffect_;
	delete modelDeathParticles_;
	delete skydome_;
	delete mapChipField_;
	delete cameraController;
	delete fade_;



	for (Enemy* enemy : enemies_) {
		delete enemy;
	}

	for (EnemyMini* enemyMini : miniEnemies_) {
		delete enemyMini;
	}

	for (ShieldEnemy* shieldEnemiy : shieldEnemies_) {
		delete shieldEnemiy;
	}

	for (Boss* boss : bosss_) {
		delete boss;
	}

	for (HitEffect* enemyDeathParticles_ : enemyDeathParticless_) {
		delete enemyDeathParticles_;
	}


	for (GuardEffect* enemyGuardParticles_ : enemyGuardParticless_) {
		delete enemyGuardParticles_;
	}

	enemies_.clear();
	shieldEnemies_.clear();
	miniEnemies_.clear();
	bosss_.clear();

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlocks : worldTransformBlockLine) {	
			delete worldTransformBlocks;
		}
	}


	worldTransformBlocks_.clear();

	delete pressSpaceSprite_;
	delete gameOverSprite_;
	delete clearSprite_;
	pressSpaceSprite_ = nullptr;
	gameOverSprite_ = nullptr;
	clearSprite_ = nullptr;


}


void GameScene::Initialize(StageManager* stageManager) {

	stageManager_ = stageManager;
	textureHandle_ = TextureManager::Load("matyacap.png");
	//soundDataHandle_ = Audio::GetInstance()->LoadWave("mokugyo.wav");

	sprite_ = Sprite::Create(textureHandle_, {100, 50});
	modelBody_ = Model::CreateFromOBJ("player", true);
	modelArm_ = Model::CreateFromOBJ("playerArm", true);
	modelTire_ = Model::CreateFromOBJ("playerTire", true);
	modelBlock_ = Model::CreateFromOBJ("block", true);
	modelSkydome_ = Model::CreateFromOBJ("skydome",true);
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);
	modelBoss_ = Model::CreateFromOBJ("boss", true);
	modelEnemyMini_ = Model::CreateFromOBJ("enemyMini", true);
	modelShieldEnemy_ = Model::CreateFromOBJ("shieldEnemy", true);
	modelDeathParticles_ = Model::CreateFromOBJ("deathParticle", true);
	modelPlayerAttackEffect_ = Model::CreateFromOBJ("hit_effect", true);
	modelEnemyAttackEffect_ = Model::CreateFromOBJ("enemyHit_effect", true);
	modelEnemyDeathEffect_ = Model::CreateFromOBJ("particle", true);
	modelEnemyGuardEffect_ = Model::CreateFromOBJ("ring", true);

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
	stageManager_->SetCurrentStageIndex(0);
	const StageManager::StageData& stageData = stageManager_->GetCurrentStageData();

	std::string stageFileName = "Resources/" + stageData.name + ".csv";
		
	mapChipField_->LoadMapChipCsv(stageFileName);

	GenerateField();

	assert(player_ != nullptr && "ステージCSVにP0がありません");
	
	//stageManager_->	SetCurrentStageIndex(0);
	//const StageManager::StageData& stageData = stageManager_->GetCurrentStageData();
		



	//std::string stageFileName = "Resources/" + stageData.name + ".csv";
	//mapChipField_->LoadMapChipCsv(stageFileName);

	//KamataEngine::Vector3 enemyPos = mapChipField_->GetMapChipPositionByIndex(5,5); 
	//enemy_ = new Enemy();
	//enemy_->Initialize(modelEnemy_, &camera_, enemyPos);






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

		GuardEffect* newEnemyGuardParticles = new GuardEffect();
		Vector3 enemyGuardParticlesPos = mapChipField_->GetMapChipPositionByIndex(20 * i, 5);
		newEnemyGuardParticles->Initialize(enemyGuardParticlesPos);
		newEnemyGuardParticles->SetCamera(&camera_);
		newEnemyGuardParticles->SetModel(modelEnemyGuardEffect_);
		
		enemyGuardParticless_.push_back(newEnemyGuardParticles);
	}

	InitializeRandom();

	reloadRequseted = false;


	gameOverTextureHandle_ = TextureManager::Load("gameOver.png");
	gameOverSprite_ = Sprite::Create(gameOverTextureHandle_, {0.0f,0.0f});

	clearTextureHandle_ = TextureManager::Load("clear.png");
	clearSprite_ = Sprite::Create(clearTextureHandle_, {0.0f,0.0f});

	pressSpaceTextureHandle_ = TextureManager::Load("plessSpaceKey.png");
	pressSpaceSprite_ = Sprite::Create(pressSpaceTextureHandle_, {320.0f, 450.0f});

	gameOverSprite_->SetSize({1280.0f, 720.0f});
	clearSprite_->SetSize({1280.0f, 720.0f});
	pressSpaceSprite_->SetSize({640.0f, 360.0f});

	resultTimer_ = 0;

	isClearBossDefeated_ = false;
}

void GameScene::Update() {

	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->GetIsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

		
	shieldEnemies_.remove_if([](ShieldEnemy* shieldEnemy) {
		if (shieldEnemy->GetIsDead()) {
			delete shieldEnemy;
			return true;
		}
		return false;
	});

	miniEnemies_.remove_if([](EnemyMini* enemyMini) {
		if (enemyMini->GetIsDead()) {
			delete enemyMini;
			return true;
		}
		return false;
	});


	 bosss_.remove_if([this](Boss* boss) {
		if (boss->GetIsDead()) {

			// =============================
			// クリア対象Bossを倒した
			// =============================
			if (boss->GetIsClearBoss()) {

				isClearBossDefeated_ = true;
			}

			delete boss;

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

	if (ImGui::Button("Reload")) {
		reloadRequseted = true;
	}

	ImGui::End();

	ImGui::ShowDemoWindow();

	#endif

}

void GameScene::Draw() {

	Sprite::PreDraw();

	//sprite_->Draw();
	Sprite::PreDraw();


	Sprite::PostDraw();


	Sprite::PostDraw();

	
	Model::PreDraw();

	//modelBody_->Draw(worldTransform_, debugCamera_->GetCamera());
	
	if (!player_->GetIsDead()) {
		player_->Draw();
	}

	skydome_->Draw();

	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	for (ShieldEnemy* shieldEnemy : shieldEnemies_) {
		shieldEnemy->Draw();
	}

	for (EnemyMini* enemyMini : miniEnemies_) {
		enemyMini->Draw();
	}

	for (Boss* boss : bosss_) {
		boss->Draw();
	}

	for (HitEffect* enemyDeathParticles : enemyDeathParticless_) {
		
		enemyDeathParticles->Draw();
	}

	for (GuardEffect* enemyGuardParticles_ : enemyGuardParticless_) {

		enemyGuardParticles_->Draw();
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


	if (phase_ == Phase::kFadeOut && fade_->IsFinished()) {

		Sprite::PreDraw();

		gameOverSprite_->Draw();
		pressSpaceSprite_->Draw();

		Sprite::PostDraw();
	}

	if (phase_ == Phase::kClear && fade_->IsFinished()) {

		Sprite::PreDraw();

		clearSprite_->Draw();
		pressSpaceSprite_->Draw();

		Sprite::PostDraw();
	}

}

void GameScene::GenerateField() {

	uint32_t numBlockVirtcal = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numBlockVirtcal);

	for (uint32_t i = 0; i < numBlockVirtcal; i++) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	
	}

	for (uint32_t y = 0; y < numBlockVirtcal; y++) {
		for (uint32_t x = 0; x < numBlockHorizontal; x++) {
			//if (mapChipField_->GetMapChipTypeByIndex(x,y) == MapChipType::kBlock) {
			//	WorldTransform* worldTransform = new WorldTransform();
			//	worldTransform->Initialize();
			//	worldTransformBlocks_[y][x] = worldTransform;
			//	worldTransformBlocks_[y][x]->translation_ = mapChipField_->GetMapChipPositionByIndex(x,y);
			//}

			switch (mapChipField_->GetMapChipTypeByIndex(x, y)) {
			    case MapChipType::kBlock: {

					WorldTransform* worldTransform = new WorldTransform();
					worldTransform->Initialize();

				    worldTransformBlocks_[y][x] = worldTransform;
				    worldTransformBlocks_[y][x]->translation_ = mapChipField_->GetMapChipPositionByIndex(x, y);
				    break;
			    }

			case MapChipType::kPlayer: {

				    player_ = new Player();
				    KamataEngine::Vector3 playerPos = mapChipField_->GetMapChipPositionByIndex(x, y);
				    player_->Initialize(modelBody_, modelArm_, modelTire_, modelPlayerAttackEffect_, modelEnemyDeathEffect_, &camera_, playerPos);
				    player_->SetMapChipField(mapChipField_);

				    deathParticles_ = new DeathParticles();
				    deathParticles_->Initialize(modelDeathParticles_, &camera_, playerPos);
				    break;
			    }

			case MapChipType::kEnemy:

				switch (mapChipField_->GetMapChipSubIDByIndex(x, y)) {

				case 0:
					CreateEnemy(x, y);
					break;

				case 1:
					CreateShieldEnemy(x, y);
					break;
				case 2:
					CreateEnemyMini(x, y);
					break;


				case 4:
					CreateBoss(x, y,true);
					break;
				}

				break;


			}

		}
	
	}


}

void GameScene::CreateEnemy(const uint32_t& x, const uint32_t& y) {
	
	Enemy* newEnemy = new Enemy();
	Vector3 enemyPos = mapChipField_->GetMapChipPositionByIndex(x,y);
	newEnemy->Initialize(modelEnemy_, &camera_, enemyPos);
	newEnemy->SetGameScene(this);
	enemies_.push_back(newEnemy);
}

void GameScene::CreateShieldEnemy(const uint32_t& x, const uint32_t& y) {
	ShieldEnemy* newShieldEnemy = new ShieldEnemy();
	Vector3 enemyPos = mapChipField_->GetMapChipPositionByIndex(x,y);
	newShieldEnemy->Initialize(modelShieldEnemy_, &camera_, enemyPos);
	newShieldEnemy->SetGameScene(this);
	shieldEnemies_.push_back(newShieldEnemy);
	
}

void GameScene::CreateEnemyMini(const uint32_t& x, const uint32_t& y) { 
	
	EnemyMini* newEnemyMini = new EnemyMini();
	Vector3 enemyPos = mapChipField_->GetMapChipPositionByIndex(x, y);
	newEnemyMini->Initialize(modelEnemyMini_, &camera_, enemyPos);
	newEnemyMini->SetGameScene(this);
	newEnemyMini->SetMapChipField(mapChipField_);
	miniEnemies_.push_back(newEnemyMini);

}

void GameScene::CreateBoss(const uint32_t& x, const uint32_t& y, bool isClearBoss) { 


	Boss* newBoss = new Boss();

	Vector3 enemyPos = mapChipField_->GetMapChipPositionByIndex(x, y);

	newBoss->Initialize(modelBoss_, modelEnemyAttackEffect_ ,& camera_, enemyPos);

	newBoss->SetGameScene(this);

	// このBossを倒したらCLEARか
	newBoss->SetIsClearBoss(isClearBoss);

	bosss_.push_back(newBoss);
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

		} else if (isClearBossDefeated_) {

			phase_ = Phase::kClear;

			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}

		break;

	case GameScene::Phase::kDeath:
		DeathParticlePhase();
		if (deathParticles_->GetIsFinished()) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		

		break;

	case GameScene::Phase::kClear:

		if (fade_->IsFinished()) {

			if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {

				finished_ = true;
			}
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

	for (ShieldEnemy* shieldEnemy : shieldEnemies_) {
		shieldEnemy->Update();
	}

	for (EnemyMini* enemyMini : miniEnemies_) {
		enemyMini->Update();
	}

	for (Boss* boss : bosss_) {
		boss->Update();
	}

	for (HitEffect* enemyDeathParticles : enemyDeathParticless_) {
		enemyDeathParticles->Update();
	}

	for (GuardEffect* enemyGuardParticles_ : enemyGuardParticless_) {
		enemyGuardParticles_->Update();
	}

	cameraController->Update();

	// カメラ行列は更新
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

	for (ShieldEnemy* shieldEnemy : shieldEnemies_) {
		shieldEnemy->Update();
	}

	for (EnemyMini* enemyMini : miniEnemies_) {
		enemyMini->Update();
	}

	for (Boss* boss : bosss_) {
		boss->Update();
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

	#pragma region 自キャラとノーマル敵キャラ当たり判定
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

	
	#pragma region 自キャラとシールド敵キャラ当たり判定
	{
		AABB aabb1, aabb2;
		aabb1 = player_->GetAABB();
		for (ShieldEnemy* shieldEnemy : shieldEnemies_) {

			if (shieldEnemy->GetIsCollisionDisabled()) {
				continue;
			}

			aabb2 = shieldEnemy->GetAABB();

			if (IsCollisionAABB2D(aabb1, aabb2)) {
				player_->OnShieldCollision(shieldEnemy);
				shieldEnemy->OnCollision(player_);
			}
		}
	}
	#pragma endregion


	#pragma region プレイヤー攻撃とMini敵

	for (EnemyMini* enemyMini : miniEnemies_) {

		if (enemyMini->GetIsCollisionDisabled()) {
			continue;
		}

		AABB enemyAABB = enemyMini->GetAABB();

		// ==============================
		// 右腕
		// ==============================

		if (player_->IsRightArmAttacking()) {

			AABB attackAABB = player_->GetRightArmAttackAABB();

			if (IsCollisionAABB2D(attackAABB, enemyAABB)) {

				enemyMini->OnAttackCollision();
				continue;
			}
		}

		// ==============================
		// 左腕
		// ==============================

		if (player_->IsLeftArmAttacking()) {

			AABB attackAABB = player_->GetLeftArmAttackAABB();

			if (IsCollisionAABB2D(attackAABB, enemyAABB)) {

				enemyMini->OnAttackCollision();
				continue;
			}
		}

		// ==============================
		// 右ChargeShot
		// ==============================

		if (player_->IsRightChargeShotActive()) {

			AABB attackAABB = player_->GetRightChargeShotAABB();

			if (IsCollisionAABB2D(attackAABB, enemyAABB)) {

				enemyMini->OnAttackCollision();
				continue;
			}
		}

		// ==============================
		// 左ChargeShot
		// ==============================

		if (player_->IsLeftChargeShotActive()) {

			AABB attackAABB = player_->GetLeftChargeShotAABB();

			if (IsCollisionAABB2D(attackAABB, enemyAABB)) {

				enemyMini->OnAttackCollision();
				continue;
			}
		}
	}

#pragma endregion

	#pragma region プレイヤー攻撃とBossu
	for (Boss* boss : bosss_) {

		if (boss->GetIsCollisionDisabled()) {
			continue;
		}

		AABB bossAABB = boss->GetAABB();

		// =================================
		// 右腕
		// =================================
		if (player_->IsRightArmAttacking()) {

			AABB attackAABB = player_->GetRightArmAttackAABB();

			if (IsCollisionAABB2D(attackAABB, bossAABB)) {

				boss->OnAttackCollision(player_->GetRightArmAttackID());
			}
		}

		// =================================
		// 左腕
		// =================================
		if (player_->IsLeftArmAttacking()) {

			AABB attackAABB = player_->GetLeftArmAttackAABB();

			if (IsCollisionAABB2D(attackAABB, bossAABB)) {

				boss->OnAttackCollision(player_->GetLeftArmAttackID());
			}
		}
		// =================================
		// 右ChargeShot
		// =================================
		if (player_->IsRightChargeShotActive()) {

			AABB attackAABB = player_->GetRightChargeShotAABB();

			if (IsCollisionAABB2D(attackAABB, bossAABB)) {

				boss->OnAttackCollision(player_->GetRightChargeShotAttackID());

				// 命中した弾を消す
				player_->DisableRightChargeShot();
			}
		}

		// =================================
		// 左ChargeShot
		// =================================
		if (player_->IsLeftChargeShotActive()) {

			AABB attackAABB = player_->GetLeftChargeShotAABB();

			if (IsCollisionAABB2D(attackAABB, bossAABB)) {

				boss->OnAttackCollision(player_->GetLeftChargeShotAttackID());

				// 命中した弾を消す
				player_->DisableLeftChargeShot();
			}
		}
	}

#pragma endregion

	#pragma region 自キャラとMini敵

	{
		AABB playerAABB = player_->GetAABB();

		for (EnemyMini* enemyMini : miniEnemies_) {

			// 死亡中などは無視
			if (enemyMini->GetIsCollisionDisabled()) {
				continue;
			}

			AABB miniAABB = enemyMini->GetAABB();

			if (IsCollisionAABB2D(playerAABB, miniAABB)) {

				// Playerだけ死亡
				player_->OnEnemyCollision();
			}
		}
	}

#pragma endregion

	#pragma region 自キャラとBoss

	{
		AABB playerAABB = player_->GetAABB();

		for (Boss* boss : bosss_) {

			if (boss->GetIsCollisionDisabled()) {
				continue;
			}

			AABB bossAABB = boss->GetAABB();

			if (IsCollisionAABB2D(playerAABB, bossAABB)) {

				// Playerだけ死亡
				player_->OnEnemyCollision();
			}
		}
	}

#pragma endregion

	#pragma region 自キャラとBoss衝撃波

	{
		AABB playerAABB = player_->GetAABB();

		for (Boss* boss : bosss_) {

			// 衝撃波が出てない
			if (!boss->IsShockWaveActive()) {
				continue;
			}

			AABB shockWaveAABB = boss->GetShockWaveAABB();

			if (IsCollisionAABB2D(playerAABB, shockWaveAABB)) {

				// Player死亡
				player_->OnEnemyCollision();

				// 当たった衝撃波は消す
				boss->DisableShockWave();
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

void GameScene::CreateGuardEffect(KamataEngine::Vector3 pos) {
	GuardEffect* newEnemyGuardParticles = GuardEffect::Create(pos);
	newEnemyGuardParticles->SetIsKnockback();
	enemyGuardParticless_.push_back(newEnemyGuardParticles);
	
}

void GameScene::CreateEnemyMini(const KamataEngine::Vector3& pos) {

	EnemyMini* newEnemyMini = new EnemyMini();

	newEnemyMini->Initialize(modelEnemyMini_, &camera_, pos);

	newEnemyMini->SetGameScene(this);
	newEnemyMini->SetMapChipField(mapChipField_);

	miniEnemies_.push_back(newEnemyMini);

}
