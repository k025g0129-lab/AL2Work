#pragma once
#include "KamataEngine.h"

class GameScene{
public:

KamataEngine::Sprite* sprite_ = nullptr;

public:

GameScene();
~GameScene();

void Initialize();

void Update();

void Draw();

private:
uint32_t textureHandle_ = 0;

};
