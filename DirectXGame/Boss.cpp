#define NOMINMAX
#include "Boss.h"
#include "KamataEngine.h"
#include <numbers> 
#include <cassert> 
#include <algorithm> 
#include "function.h"
#include "Player.h"
#include "GameScene.h"


void Boss::Initialize(KamataEngine::Model* model, KamataEngine::Model* attackModel, KamataEngine::Camera* camera, const KamataEngine::Vector3 pos) {
	assert(model);
	model_ = model;
	assert(attackModel);
	attackModel_ = attackModel;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;

	//worldTransform_.rotation_.y = (std::numbers::pi_v<float> / 2.0f) * 3.0f;


	velocity_ = {-kWalkSpeed, 0, 0};

	walkTimer_ = 0.0f;
	life = 3;
	hitAttackIDs_.clear();

	groundY_ = pos.y;

	attackTimer_ = 0;
	jumpTimer_ = 0;

	attackPhase_ = AttackPhase::kWait;

	isShockWaveActive_ = false;
	shockWaveTimer_ = 0;
	isKnockbackRequest = false;

	shockWaveTransform_.Initialize();
	shockWaveTransform_.translation_ = pos;
	shockWaveTransform_.rotation_.y = (std::numbers::pi_v<float>/ 2.0f) * 3.0f;
	
}

void Boss::Update() {

	if (isKnockbackRequest) {

		behaviorRequest_ = Behavior::kKnockback;

		isKnockbackRequest = false;
	}


	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {

		case Boss::Behavior::kMove:
			BehaviorMoveInitialize();
			// worldTransform_.rotation_.y = (std::numbers::pi_v<float> / 2.0f) * 3.0f;
			break;

		case Boss::Behavior::kDamageReaction:
			BehaviorkDamageReactionInitialize();
			break;

		case Boss::Behavior::kKnockback:
			BehaviorKnockbackInitialize();
			break;

		case Boss::Behavior::kDeath:
			BehaviorDeathInitialize();
			// deathParameter_ = 0;
			break;

		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_B)) {
		AddSwingImpact();
	}

	switch (behavior_) {
	case Boss::Behavior::kMove:

		AttackUpdata();
			
		// 0度へ戻ろうとする力	
		swingVelocity_ += -swingAngle_ * kSwingSpring;

		// 徐々に減衰
		swingVelocity_ *= kSwingDamping;	

		// 角度更新
		swingAngle_ += swingVelocity_;

		// 実際の回転
		worldTransform_.rotation_.z = swingAngle_;

		// 一定以上倒れたらダメージ
		if (std::abs(swingAngle_) >= kDeathSwingAngle) {
			behaviorRequest_ = Behavior::kDamageReaction;
			life--;

			if (life <= 0) {
				behaviorRequest_ = Behavior::kDeath;

			}

		}

		break;


	case Boss::Behavior::kDamageReaction: {

		damageReactionParameter_++;

		float t = float(damageReactionParameter_) / float(kDamageReactionTime);

		t = std::clamp(t, 0.0f, 1.0f);

		// 回転角度
		float theta = std::numbers::pi_v<float> * 2.0f * kDamageReactionRotateCount * t;

		// 徐々に半径を小さくする
		float radius = damageReactionStartAngle_ * (1.0f - t);

		// ==================================
		// 渦を巻く
		// ==================================
		worldTransform_.rotation_.x = sinf(theta) * radius;

		worldTransform_.rotation_.z = -cosf(theta) * radius;

		//3回転
		float faceTheta = std::numbers::pi_v<float> * 2.0f * kDamageReactionFaceRotateCount * t;
		worldTransform_.rotation_.y = damageReactionStartY_ + faceTheta;


		// 終了
		if (damageReactionParameter_ >= kDamageReactionTime) {

			// 完全に直立
			worldTransform_.rotation_.x = 0.0f;
			worldTransform_.rotation_.z = 0.0f;
			worldTransform_.rotation_.y = damageReactionStartY_;

			// 内部の揺れも完全リセット
			swingAngle_ = 0.0f;
			swingVelocity_ = 0.0f;

			behaviorRequest_ = Behavior::kMove;
		}

		break;
	}

	case Boss::Behavior::kKnockback:

		KnockbackUpdata();

		break;

	case Boss::Behavior::kDeath:

		deathParameter_++;
		worldTransform_.rotation_.x += 1.0f;
		worldTransform_.rotation_.y += 1.0f;
		worldTransform_.rotation_.z += 1.0f;
		if (deathParameter_ >= deathTime) {

			isDead_ = true;
		}

		break;
	}

	ShockWaveUpdata();

	MakeAffineMatrix(&worldTransform_);


}

void Boss::KnockbackUpdata() {
	knockbackTimer_++;

	switch (knockbackStatus) {
	case Boss::KnockbackStatus::kBackward:

		worldTransform_.rotation_.x = EaseInSine(0.0f, -(std::numbers::pi_v<float> / 2.0f) * 1.0f, knockbackTimer_, kKnockbackMotionTime);


		if (knockbackTimer_ >= kKnockbackMotionTime) {
			knockbackTimer_ = 0.0f;
			knockbackStatus = KnockbackStatus::kForward;
		}

		break;
	case Boss::KnockbackStatus::kForward:

		worldTransform_.rotation_.x = EaseInSine(-(std::numbers::pi_v<float> / 2.0f) * 1.0f, 0.0f, knockbackTimer_, kKnockbackMotionTime);


		if (knockbackTimer_ >= kKnockbackMotionTime) {

			knockbackTimer_ = 0.0f;
			knockbackStatus = KnockbackStatus::kBackward;

			behaviorRequest_ = Behavior::kMove;
		}

		break;

	}


}

void Boss::AttackUpdata() {

switch (attackPhase_) {

	case AttackPhase::kWait:

		attackTimer_++;

		if (attackTimer_ >= kAttackInterval) {

			attackTimer_ = 0;
			jumpTimer_ = 0;

			attackPhase_ = AttackPhase::kRise;
		}

		break;

	case AttackPhase::kRise: {

		jumpTimer_++;

		float t = float(jumpTimer_) / float(kRiseTime);

		t = std::clamp(t, 0.0f, 1.0f);

		// 上昇
		worldTransform_.translation_.y = groundY_ + kJumpHeight * t;

		// 上昇中ゆらゆら
		worldTransform_.rotation_.z = std::sin(float(jumpTimer_) * 0.25f) * 0.2f;

		if (jumpTimer_ >= kRiseTime) {

			jumpTimer_ = 0;

			attackPhase_ = AttackPhase::kFall;
		}

		break;
	}

		case AttackPhase::kFall: {

			jumpTimer_++;

			float t = float(jumpTimer_) / float(kFallTime);

			t = std::clamp(t, 0.0f, 1.0f);

			// 上から地面へ
			worldTransform_.translation_.y = groundY_ + kJumpHeight * (1.0f - t);

			worldTransform_.rotation_.z = std::sin(float(jumpTimer_) * 0.35f) * 0.2f;

			if (jumpTimer_ >= kFallTime) {

				worldTransform_.translation_.y = groundY_;

				worldTransform_.rotation_.z = 0.0f;

				jumpTimer_ = 0;

				// ============================
				// 着地 → 衝撃波
				// ============================

				shockWaveTransform_.translation_ = worldTransform_.translation_;

				shockWaveTransform_.translation_.y += 0.5f;

				shockWaveTimer_ = 0;

				isShockWaveActive_ = true;

				attackPhase_ = AttackPhase::kWait;
			}

			break;
		}
	}
	


}

void Boss::ShockWaveUpdata() {
	// ========================================
	// 衝撃波
	// ========================================
	if (isShockWaveActive_) {

		shockWaveTimer_++;

		// Bossは右側なので左へ飛ばす
		shockWaveTransform_.translation_.x -= kShockWaveSpeed;

		// ちょっと回転させる
		//shockWaveTransform_.rotation_.z += 0.2f;

		if (shockWaveTimer_ >= kShockWaveLifeTime) {

			isShockWaveActive_ = false;

			shockWaveTimer_ = 0;
		}
	}

	MakeAffineMatrix(&shockWaveTransform_);
}

void Boss::Draw() {

	model_->Draw(worldTransform_, *camera_);

	if (isShockWaveActive_) {

		attackModel_->Draw(shockWaveTransform_, *camera_);
	}

}

KamataEngine::Vector3 Boss::GetWorldPos() { 
	
	KamataEngine::Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB Boss::GetAABB() {

	KamataEngine::Vector3 worldPos = GetWorldPos();

	// 判定だけ上へ
	worldPos.y += kCollisionOffsetY;

	AABB aabb;

	aabb.min = {worldPos.x - kWidth, worldPos.y - kHeigth, worldPos.z - kWidth};

	aabb.max = {worldPos.x + kWidth, worldPos.y + kHeigth, worldPos.z + kWidth};

	return aabb;
}

void Boss::OnCollision(Player* player) {

	if (behavior_ == Behavior::kDeath) {
		return;
	}

	if (player->isDash()) {

		//向き合っているか
		if (player->GetLRDirection() != lrDirection_) {
			KamataEngine::Vector3 effectPos = (Vector3Add(worldTransform_.translation_, player->GetWorldTransform().translation_));
			effectPos.x /= 2.0f;
			effectPos.y /= 2.0f;
			effectPos.z /= 2.0f;
			gameScene_->CreateGuardEffect(effectPos);
			isBehaviorRequestKnockback();

			player->isBehaviorRequestKnockback();
			
			return;
		}

		isCollisionDisabled_ = true;
		behaviorRequest_ = Behavior::kDeath;
	}

	KamataEngine::Vector3 effectPos = (Vector3Add(worldTransform_.translation_, player->GetWorldTransform().translation_));
	effectPos.x /= 2.0f;
	effectPos.y /= 2.0f;
	effectPos.z /= 2.0f;

	gameScene_->CreateHitEffect(effectPos);


}

void Boss::OnAttackCollision(uint32_t attackID) {

	if (attackPhase_ == AttackPhase::kRise || attackPhase_ == AttackPhase::kFall) {

		return;
	}

	if (behavior_ == Behavior::kDeath) {
		return;
	}

	if (std::find(hitAttackIDs_.begin(), hitAttackIDs_.end(), attackID) != hitAttackIDs_.end()) {

		return;
	}

    // 初めて受けたIDなので記録
	hitAttackIDs_.push_back(attackID);



	AddSwingImpact();

	gameScene_->CreateHitEffect(worldTransform_.translation_);




}

void Boss::BehaviorMoveInitialize() {
	////worldTransform_.rotation_.x = 0.0f;
	//worldTransform_.rotation_.y = (std::numbers::pi_v<float> / 2.0f) * 3.0f;
	//worldTransform_.rotation_.z = 0.0f;
	worldTransform_.rotation_.x = 0.0f;
	worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 4.0f;
	worldTransform_.rotation_.z = 0.0f;

	swingAngle_ = 0.0f;
	swingVelocity_ = 0.0f;

}

void Boss::BehaviorkDamageReactionInitialize() {
	damageReactionParameter_ = 0;
	damageReactionStartAngle_ = std::abs(swingAngle_);
	swingVelocity_ = 0.0f;

    damageReactionStartY_ = worldTransform_.rotation_.y;
	swingVelocity_ = 0.0f;
}

void Boss::BehaviorKnockbackInitialize() {

	worldTransform_.rotation_.x = 0.0f;
	worldTransform_.rotation_.y = (std::numbers::pi_v<float> / 2.0f) * 3.0f;
	worldTransform_.rotation_.z = 0.0f;

	knockbackStatus = KnockbackStatus::kBackward;
	knockbackTimer_ = 0.0f;

}

void Boss::BehaviorDeathInitialize() {
	deathParameter_ = 0; 

}

void Boss::AddSwingImpact() { 
	swingVelocity_ -= kHitSwingPower;

}

AABB Boss::GetShockWaveAABB() { 
	KamataEngine::Vector3 pos = shockWaveTransform_.translation_;

	AABB aabb;

	aabb.min = {pos.x - kShockWaveWidth, pos.y - kShockWaveHeight, pos.z - kShockWaveWidth};

	aabb.max = {pos.x + kShockWaveWidth, pos.y + kShockWaveHeight, pos.z + kShockWaveWidth};

	return aabb;
}

