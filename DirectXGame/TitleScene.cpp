#include "TitleScene.h"
#include <numbers> 
#include "function.h"



TitleScene::~TitleScene() { 
	delete model_; 
}

void TitleScene::Initialize() {

	finished_ = false; 

	model_ = KamataEngine::Model::CreateFromOBJ("player", true);


	camera_.Initialize();
	worldTransform_.Initialize();
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;
	worldTransform_.translation_.z = -40.0f;
	KamataEngine::PrimitiveDrawer::GetInstance()->SetCamera(&camera_);
}

void TitleScene::Update() {

	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		finished_ = true;
	}
	worldTransform_.rotation_.y += 0.05f;
	//行列計算
	MakeAffineMatrix(&worldTransform_);
}

void TitleScene::Draw() {
	KamataEngine::Model::PreDraw();
	model_->Draw(worldTransform_, camera_);
	KamataEngine::Model::PostDraw();
}
