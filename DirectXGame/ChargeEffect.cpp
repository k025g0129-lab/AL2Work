#define NOMINMAX

#include "ChargeEffect.h"
#include "function.h"

#include <cassert>
#include <cmath>
#include <numbers>

void ChargeEffect::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {

	assert(model);

	model_ = model;

	camera_ = camera;

	// ========================================
	// 色
	// ========================================

	color_.Initialize();

	// 通常チャージ色
	color_.SetColor({0.2f, 0.7f, 1.0f, 1.0f});

	spawnTimer_ = 0;

	// ========================================
	// 全Particle初期化
	// ========================================

	for (Particle& particle : particles_) {

		particle.worldTransform.Initialize();

		particle.worldTransform.scale_ = {0.0f, 0.0f, 0.0f};

		particle.angle = 0.0f;

		particle.radius = 0.0f;

		particle.startRadius = 0.0f;

		particle.rotateSpeed = 0.0f;

		particle.isActive = false;
	}
}

void ChargeEffect::Update(const KamataEngine::Vector3& playerPos, bool isCharging, bool isFullCharge) {

	// ========================================
	// チャージしてない
	// ========================================

	if (!isCharging) {

		spawnTimer_ = 0;

		// 全部消す
		for (Particle& particle : particles_) {

			particle.isActive = false;

			particle.worldTransform.scale_ = {0.0f, 0.0f, 0.0f};
		}

		return;
	}

	// ========================================
	// 色変更
	// ========================================

	if (isFullCharge) {

		// フルチャージ
		// 赤～オレンジ
		color_.SetColor({0.9f, 0.2f, 0.1f, 1.0f});

	} else {

		// 通常チャージ
		// 青
		color_.SetColor({0.0f, 1.0f, 0.0f, 1.0f});
	}

	// ========================================
	// 新しい粒生成
	// ========================================

	spawnTimer_++;

	if (spawnTimer_ >= kSpawnInterval) {

		spawnTimer_ = 0;

		SpawnParticle(playerPos);
	}

	// ========================================
	// 粒更新
	// ========================================

	for (Particle& particle : particles_) {

		if (!particle.isActive) {

			continue;
		}

		// ========================================
		// Playerの周りを回す
		// ========================================

		particle.angle += particle.rotateSpeed;

		// ========================================
		// 徐々に中心へ
		// ========================================

		particle.radius -= kApproachSpeed;

		// ========================================
		// 中心まで来た
		// ========================================

		if (particle.radius <= 0.1f) {

			particle.isActive = false;

			continue;
		}

		// ========================================
		// 現在位置
		// ========================================

		particle.worldTransform.translation_.x = playerPos.x + std::cos(particle.angle) * particle.radius;

		particle.worldTransform.translation_.y = playerPos.y + std::sin(particle.angle) * particle.radius;

		particle.worldTransform.translation_.z = playerPos.z;

		// ========================================
		// 中心へ行くほど小さくする
		// ========================================

		float scaleRate = particle.radius / particle.startRadius;

		float scale = kParticleScale * scaleRate;

		particle.worldTransform.scale_ = {scale, scale, scale};

		// ========================================
		// 行列
		// ========================================

		MakeAffineMatrix(&particle.worldTransform);
	}
}

void ChargeEffect::Draw() {

	if (model_ == nullptr || camera_ == nullptr) {

		return;
	}

	for (Particle& particle : particles_) {

		if (!particle.isActive) {

			continue;
		}

		model_->Draw(particle.worldTransform, *camera_, &color_);
	}
}

void ChargeEffect::SpawnParticle(const KamataEngine::Vector3& playerPos) {

	// 空いているParticleを探す
	for (Particle& particle : particles_) {

		if (particle.isActive) {

			continue;
		}

		// ========================================
		// ランダム角度
		// ========================================

		particle.angle = RandomFloat(0.0f, std::numbers::pi_v<float> * 2.0f);

		// ========================================
		// Playerからランダムな距離
		// ========================================

		particle.startRadius = RandomFloat(kMinSpawnRadius, kMaxSpawnRadius);

		particle.radius = particle.startRadius;

		// ========================================
		// 粒ごとに回転速度を少し変える
		// ========================================

		particle.rotateSpeed = RandomFloat(0.06f, 0.12f);

		// ========================================
		// 最初の位置
		// ========================================

		particle.worldTransform.translation_.x = playerPos.x + std::cos(particle.angle) * particle.radius;

		particle.worldTransform.translation_.y = playerPos.y + std::sin(particle.angle) * particle.radius;

		particle.worldTransform.translation_.z = playerPos.z;

		// ========================================
		// 最初のサイズ
		// ========================================

		particle.worldTransform.scale_ = {kParticleScale, kParticleScale, kParticleScale};

		particle.isActive = true;

		// 1回につき1粒だけ
		return;
	}
}