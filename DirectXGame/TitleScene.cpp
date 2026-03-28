#include "TitleScene.h"
#include <numbers> 
#include "function.h"



TitleScene::~TitleScene() { 
	delete model_; 
	delete fade_;
}

void TitleScene::Initialize() {

	finished_ = false; 

	model_ = KamataEngine::Model::CreateFromOBJ("player", true);
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn,1.0f);

	camera_.Initialize();
	worldTransform_.Initialize();
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;
	worldTransform_.translation_.z = -40.0f;
	KamataEngine::PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	phase_ = Phase::kFadeIn;
}

void TitleScene::Update() {


	switch (phase_) {

	case TitleScene::Phase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;

	case TitleScene::Phase::kMain:
		fade_->Stop();
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;

	case TitleScene::Phase::kFadeOut:
		if (fade_->IsFinished() ) {
			finished_ = true;
		}
		break;
	}

	worldTransform_.rotation_.y += 0.05f;
	//行列計算
	MakeAffineMatrix(&worldTransform_);

	fade_->Update();
}

void TitleScene::Draw() {
	KamataEngine::Model::PreDraw();
	model_->Draw(worldTransform_, camera_);
	KamataEngine::Model::PostDraw();

	fade_->Draw();
}
