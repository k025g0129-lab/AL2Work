#pragma once
#include "KamataEngine.h"

class Player;


class CameraController {

public:
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};




public:
	void Initialize(KamataEngine::Camera* camera);
	void Update();
	void Draw();

	
	void Reset();

	void SetTarget(Player* target) { target_ = target; };
	void SetMovebleArea(Rect area) { movableArea_ = area; };

private:

	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	Player* target_ = nullptr;

	KamataEngine::Vector3 targetOffset_ = {0, 0, -15.0f};

	Rect movableArea_ = {0, 100, 0, 100};

	KamataEngine::Vector3 targetCameraPos;
	static inline const float kInterpolationRate = 0.5f;

	static inline const float kVelocityBias = 0.5f;

	static inline const Rect margin = {-50,50,-10,10};
};
