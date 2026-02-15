#include "GameScene.h"
#include "2d/ImGuiManager.h"

using namespace KamataEngine;

GameScene::GameScene() { 

	Initialize();

}

GameScene::~GameScene() { 
	delete sprite_; 
	delete model_;
	delete debugCamera_;
	delete player_;
}

void GameScene::Initialize() {
	textureHandle_ = TextureManager::Load("matyacap.png");
	//soundDataHandle_ = Audio::GetInstance()->LoadWave("mokugyo.wav");

	sprite_ = Sprite::Create(textureHandle_, {100, 50});
	model_ = Model::Create();

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
}

void GameScene::Update() {
	Vector2 pos = sprite_->GetPosition();
	pos.x += 2.0f;
	pos.y += 1.0f;

	sprite_->SetPosition(pos);
	debugCamera_->Update();

	player_->Update();

	#ifdef _DEBUG

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		Audio::GetInstance()->StopWave(voiceHandle_);
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
	
	PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});
			


	Model::PostDraw();


}
