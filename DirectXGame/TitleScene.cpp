#include "TitleScene.h"
#include <numbers> 
#include "function.h"



TitleScene::~TitleScene() { 
	delete model_; 
	delete fade_;
	delete modelTitleLogo_;

	delete pressSpaceSprite_;
	modelTitleLogo_ = nullptr;
	pressSpaceSprite_ = nullptr;

}

void TitleScene::Initialize() {

	finished_ = false; 

	model_ = KamataEngine::Model::CreateFromOBJ("player", true);
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn,1.0f);

	modelTitleLogo_ = KamataEngine::Model::CreateFromOBJ("titleLogo", true);
	worldTransformTitleLogo_.Initialize();
	worldTransformTitleLogo_.translation_ = {0.0f, 0.0f, -46.0f};
	worldTransformTitleLogo_.rotation_.z = std::numbers::pi_v<float>;
	worldTransformTitleLogo_.rotation_.x = std::numbers::pi_v<float>;
	titleLogoBaseY_ = worldTransformTitleLogo_.translation_.y;
	titleLogoTimer_ = 0.0f;
	MakeAffineMatrix(&worldTransformTitleLogo_);

	camera_.Initialize();
	worldTransform_.Initialize();
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;
	worldTransform_.translation_.z = -40.0f;
	KamataEngine::PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	pressSpaceTextureHandle_ = KamataEngine::TextureManager::Load("plessSpaceKey.png");
	pressSpaceSprite_ = KamataEngine::Sprite::Create(pressSpaceTextureHandle_, {320.0f, 450.0f});
	pressSpaceSprite_->SetSize({640.0f, 360.0f});

	zimenTextureHandle_ = KamataEngine::TextureManager::Load("zimen.png");
	zimenSprite_ = KamataEngine::Sprite::Create(zimenTextureHandle_, {0.0f, 0.0f});
	zimenSprite_->SetSize({1280.0f, 1080.0f});

	phase_ = Phase::kFadeIn;
}

void TitleScene::Update() {

	titleLogoTimer_ += 1.0f / 60.0f;


	worldTransformTitleLogo_.translation_.y = titleLogoBaseY_ + sinf(titleLogoTimer_ * kTitleLogoFloatSpeed) * kTitleLogoFloatRange;
	worldTransformTitleLogo_.rotation_.y = sinf(titleLogoTimer_ * 1.0f) * 0.5f;
	MakeAffineMatrix(&worldTransformTitleLogo_);

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

		// 2D
	KamataEngine::Sprite::PreDraw();

	zimenSprite_->Draw();

	KamataEngine::Sprite::PostDraw();

	//3D
	KamataEngine::Model::PreDraw();
	//model_->Draw(worldTransform_, camera_);
	modelTitleLogo_->Draw(worldTransformTitleLogo_, camera_);

	KamataEngine::Model::PostDraw();

	//2D
	KamataEngine::Sprite::PreDraw();

	pressSpaceSprite_->Draw();

	KamataEngine::Sprite::PostDraw();

	fade_->Draw();
}
