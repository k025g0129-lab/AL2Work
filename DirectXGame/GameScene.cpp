#include "GameScene.h"
#include "2d/ImGuiManager.h"
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
	delete block_;

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
	model_ = Model::Create();
	block_ = Model::Create();

	worldTransform_.Initialize();
	camera_.Initialize();

	//Audio::GetInstance()->PlayWave(soundDataHandle_);
	//voiceHandle_ = Audio::GetInstance()->PlayWave(soundDataHandle_, true);

	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	debugCamera_ = new DebugCamera(1280,720);
	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	player_ = new Player();
	player_->Initialize(model_,textureHandle_,&camera_);

	const uint32_t kNumBlockVirtical = 10;
	const uint32_t kNumBlockHorizontal = 20;
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	worldTransformBlocks_.resize(kNumBlockVirtical);

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
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
	}

}

void GameScene::Update() {
	Vector2 pos = sprite_->GetPosition();
	pos.x += 2.0f;
	pos.y += 1.0f;

	sprite_->SetPosition(pos);

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;

		camera_.TransferMatrix();

	} else {
	
			camera_.UpdateMatrix();
	}


	player_->Update();


	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
	
		for (WorldTransform* worldTransformBlocks : worldTransformBlockLine) {
				if (!worldTransformBlocks) {
					continue;
				}

				Matrix4x4 MatrixInitialize = {};
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 4; j++) {
						MatrixInitialize.m[i][j] = 0.0f; 
					}
				}

				for (int i = 0; i < 4; i++) {
					MatrixInitialize.m[i][i] = 1.0f;
				}

				Matrix4x4 afin = MatrixInitialize;
		
				Matrix4x4 S = MatrixInitialize;
				S.m[0][0] = worldTransformBlocks->scale_.x;
				S.m[1][1] = worldTransformBlocks->scale_.y;
				S.m[2][2] = worldTransformBlocks->scale_.z;

				Matrix4x4 T = MatrixInitialize;
				T.m[3][0] = worldTransformBlocks->translation_.x;
				T.m[3][1] = worldTransformBlocks->translation_.y;
				T.m[3][2] = worldTransformBlocks->translation_.z;

				Matrix4x4 Rx = MatrixInitialize;
				Rx.m[0][0] = 1.0f;
				Rx.m[1][1] = cosf(worldTransformBlocks->rotation_.x);
				Rx.m[2][2] = cosf(worldTransformBlocks->rotation_.x);
				Rx.m[1][2] = sinf(worldTransformBlocks->rotation_.x);
				Rx.m[2][1] = -sinf(worldTransformBlocks->rotation_.x);
				Matrix4x4 Ry = MatrixInitialize;
				Ry.m[1][1] = 1.0f;
				Ry.m[0][0] = cosf(worldTransformBlocks->rotation_.y);
				Ry.m[2][2] = cosf(worldTransformBlocks->rotation_.y);
				Ry.m[2][0] = sinf(worldTransformBlocks->rotation_.y);
				Ry.m[0][2] = -sinf(worldTransformBlocks->rotation_.y);
				Matrix4x4 Rz = MatrixInitialize;
				Rz.m[2][2] = 1.0f;
				Rz.m[0][0] = cosf(worldTransformBlocks->rotation_.z);
				Rz.m[1][1] = cosf(worldTransformBlocks->rotation_.z);
				Rz.m[1][0] = -sinf(worldTransformBlocks->rotation_.z);
				Rz.m[0][1] = sinf(worldTransformBlocks->rotation_.z);


				Matrix4x4 R = MatrixInitialize;
				R = Multiply(Rx, Multiply(Ry, Rz));
				afin = Multiply(S, Multiply(R, T));


				worldTransformBlocks->matWorld_ = afin;
			
				worldTransformBlocks->TransferMatrix();

		}
	}



	

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

	model_->Draw(worldTransform_, debugCamera_->GetCamera(), textureHandle_);
	player_->Draw();


	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {

		for (WorldTransform* worldTransformBlocks : worldTransformBlockLine) {
			if (!worldTransformBlocks) {
				continue;
			}

			block_->Draw(*worldTransformBlocks, camera_, textureHandle_);
		}
	
	}

	PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});
			


	Model::PostDraw();


}

