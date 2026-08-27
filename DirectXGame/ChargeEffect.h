#pragma once

#include "KamataEngine.h"
#include <array>

class ChargeEffect {

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera);

	void Update(const KamataEngine::Vector3& playerPos, bool isCharging, bool isFullCharge);

	void Draw();

private:
	// ========================================
	// 1個のチャージ粒
	// ========================================
	struct Particle {

		// 本体
		KamataEngine::WorldTransform worldTransform;

		// 縁取り用
		KamataEngine::WorldTransform outlineWorldTransform;

		// Player周囲の角度
		float angle = 0.0f;

		// Playerからの現在距離
		float radius = 0.0f;

		// 出現した時の距離
		float startRadius = 0.0f;

		// 回転速度
		float rotateSpeed = 0.0f;

		bool isActive = false;
	};

	void SpawnParticle(const KamataEngine::Vector3& playerPos);

private:
	// 粒数
	static inline const uint32_t kParticleNum = 32;

	std::array<Particle, kParticleNum> particles_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	// 全粒共通の色
	KamataEngine::ObjectColor color_;
	KamataEngine::ObjectColor outlineColor_;
	// 粒生成タイマー
	uint32_t spawnTimer_ = 0;

	// 何フレームごとに粒を出すか
	static inline const uint32_t kSpawnInterval = 2;

	// 中心へ近づく速度
	static inline const float kApproachSpeed = 0.07f;

	// 最初の粒サイズ
	static inline const float kParticleScale = 1.0f;
	static inline const float kOutlineScale = 1.25f;

	// Playerから出現する距離
	static inline const float kMinSpawnRadius = 2.0f;
	static inline const float kMaxSpawnRadius = 3.0f;
};