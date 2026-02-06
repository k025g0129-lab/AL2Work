#include "GameScene.h"

using namespace KamataEngine;

GameScene::GameScene() { 

	Initialize();

}

GameScene::~GameScene() { 
	delete sprite_; 
}

void GameScene::Initialize() {
	textureHandle_ = TextureManager::Load("uvChecker.png");
	sprite_ = Sprite::Create(textureHandle_, {100, 50});

}

void GameScene::Update() {

}

void GameScene::Draw() {

	Sprite::PreDraw();

	sprite_->Draw();

	Sprite::PostDraw();


}
