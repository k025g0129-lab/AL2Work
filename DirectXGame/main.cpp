#include <Windows.h>
#include "KamataEngine.h"
#include "GameScene.h"
#include "TitleScene.h"



using namespace KamataEngine;
TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;

enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,

};

Scene scene = Scene::kTitle;

void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();

		break;
	case Scene::kGame:
		gameScene->Update();
		break;

	}

};

void DrawScene() {

	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();

		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	}

}

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->GetIsFinished()) {
			scene = Scene::kGame;
			delete titleScene;
			titleScene = nullptr;

			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->GetIsFinished()) {
			scene = Scene::kTitle;
			delete gameScene;
			gameScene = nullptr;

			titleScene = new TitleScene;
			titleScene->Initialize();
		}

		break;
	}

};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {


	KamataEngine::Initialize(L"LC1B_19_シンモト_キョウスケ_AL2");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	ImGuiManager* imGuiManager = ImGuiManager::GetInstance();

	gameScene = new GameScene();
	titleScene = new TitleScene();


	titleScene->Initialize();
	gameScene->Initialize();


	while (true) {

		if (KamataEngine::Update()) {
			break;
		}

	imGuiManager->Begin();

		ChangeScene();
		UpdateScene();

	imGuiManager->End();



		//ここから描画
		dxCommon->PreDraw();
		
		DrawScene();

		AxisIndicator::GetInstance()->Draw();

		imGuiManager->Draw();

		dxCommon->PostDraw();
	}

	delete titleScene;
	delete gameScene;

	titleScene = nullptr;
	gameScene = nullptr;

	KamataEngine::Finalize();

	return 0;
}
