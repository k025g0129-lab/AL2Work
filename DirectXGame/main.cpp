#include <Windows.h>
#include "KamataEngine.h"
#include "GameScene.h"

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	GameScene* gameScene = new GameScene();

	KamataEngine::Initialize(L"LC1B_19_シンモト_キョウスケ_AL2");
	gameScene->Initialize();

	while (true) {

		if (KamataEngine::Update()) {
			break;
		}

	 gameScene->Update();

		//ここから描画
		dxCommon->PreDraw();
		
		gameScene->Draw();

		dxCommon->PostDraw();
	}


	delete gameScene;
	gameScene = nullptr;

	KamataEngine::Finalize();

	return 0;
}
