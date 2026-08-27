#define NOMINMAX
#include "Player.h"
#include "KamataEngine.h"
#include <numbers> 
#include <cassert> 
#include <algorithm> 
#include "function.h"


Player::Player() { 
	
}

Player::~Player() {
	delete chargeEffect_;

	chargeEffect_ = nullptr;
}

void Player::Initialize(
    KamataEngine::Model* model, KamataEngine::Model* modelArm, KamataEngine::Model* modelTire, KamataEngine::Model* modelAttack, KamataEngine::Model* modelChargeEffect, KamataEngine::Camera* camera,
    const KamataEngine::Vector3 pos) { 

	assert(model);
	model_ = model;		
	camera_ = camera;

	assert(modelArm);
	modelArm_ = modelArm;	

	assert(modelAttack);
	modelAttack_ = modelAttack;

	assert(modelTire);
	modelTire_ = modelTire;

	assert(modelChargeEffect);
	modelChargeEffect = modelChargeEffect;

	worldTransform_.Initialize();
	worldTransformRightArm_.Initialize();
	worldTransformLeftArm_.Initialize();

	worldTransform_.translation_ = pos;	
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	armUpdata();

	worldTransformDash_.Initialize();
	worldTransformDash_.translation_ = pos;
	worldTransformRightChargeShot_.Initialize();
	worldTransformRightChargeShot_.translation_ = pos;
	worldTransformLeftChargeShot_.Initialize();
	worldTransformLeftChargeShot_.translation_ = pos;
	worldTransformTire_.Initialize();
	worldTransformTire_.translation_ = pos;	

	isLeftChargeShot = false;
	isRightChargeShot = false;

	worldTransformDash_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	armRightState = ArmState::kIdle;
	armLeftState = ArmState::kIdle;

	lRDLeftChargeShot = LRDirection::kRight;
	lRDRightChargeShot = LRDirection::kRight;


	attackIDCounter_ = 0;
	rightArmAttackID_ = 0;
	leftArmAttackID_ = 0;
	rightChargeShotAttackID_ = 0;
	leftChargeShotAttackID_ = 0;
	tireRotation_ = 0.0f;

		// ========================================
	// チャージエフェクト
	// ========================================

	chargeEffect_ = new ChargeEffect();

	chargeEffect_->Initialize(modelChargeEffect, camera_);
}

void Player::Update() {
	float prePlayerX = worldTransform_.translation_.x;
	
	//========================================================================================
	//
	//========================================================================================	

	/*bool landing = false;

	if (velocity_.y < 0) {
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}


	//地についているか
	if (onGround_) {

		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}

		
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT) || KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {
	
			KamataEngine::Vector3 acceleration = {};

			if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT)) {

				if (velocity_.x <0.0f) {
					velocity_.x *= (1.0f - kAcceleration);
				}

				acceleration.x += kAcceleration;

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
		
			} else if (KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {

				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAcceleration);
				}

				acceleration.x -= kAcceleration;
		
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}

			}

			velocity_.x += acceleration.x;
			velocity_.y += acceleration.y;
			velocity_.z += acceleration.z;

			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
	
		} else {
			velocity_.x *= (1.0f - kAcceleration);
		}

		if (KamataEngine::Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_.y += kJumpAcceleration;
		}

	}*/




	//========================================================================================
	//以下は上記を関数化したのもである
	//========================================================================================	
	//攻撃ギミック作成の為以下を一時的にコメント化
	/*
	//1移動入力
	MovementInput();

	//2移動量加味して衝突判定
	CollisionMaPInfo collisionMaPInfo;
	collisionMaPInfo.moveAmount = velocity_;
	MapCollisionDetection(collisionMaPInfo);

	//3判定結果を反映して移動
	JudgmentMovement(collisionMaPInfo);

	//4天井接触での処理
	CeilingContactDetection(collisionMaPInfo);

	//5壁に接触してる処理
	WallDetection(collisionMaPInfo);

	//6接地状態の切り替え処理
	GroundStateSwitching(collisionMaPInfo);

	//7旋回制御
	TurningControl();

	//8行列計算
	MakeAffineMatrix(&worldTransform_);
*/


	if (isKnockbackRequest) {
		behaviorRequest_ = Behavior::kKnockback;

		isKnockbackRequest = false;
	}
		
	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {
		case Player::Behavior::kRoot:
			BehaviorRootInitialize();
			break;
		case Player::Behavior::kDash:
			BehaviorDashInitialize();
			break;
		case Player::Behavior::kKnockback:
			BehaviorKnockbackInitialize();
			break;
		}
		behaviorRequest_ = Behavior::kUnknown;
	
	}
	
	switch (behavior_) {
	case Player::Behavior::kRoot:

		BehaviorRootUpdate();
		break;


	case Player::Behavior::kDash:

		BehaviorDashUpdate();
		break;


	case Player::Behavior::kKnockback:

		BehaviorKnockbackUpdate();
		break;

	}

	AttackUpdate();

	// ========================================
	// チャージエフェクト
	// ========================================

	bool isCharging = false;

	// Xを現在押している
	if (KamataEngine::Input::GetInstance()->PushKey(DIK_X)) {

		// パンチ受付時間を越えている
		if (attackHoldFrame_ >= kPunchAttackFrame) {

			// 実際に腕がチャージ中
			if (armRightState == ArmState::kCharge || armLeftState == ArmState::kCharge) {

				isCharging = true;
			}
		}
	}

	// ========================================
	// フルチャージ
	// ========================================

	bool isFullCharge = false;

	if (isCharging && attackHoldFrame_ >= kChargeAttackFrame) {

		isFullCharge = true;
	}

	// ========================================
	// ChargeEffect更新
	// ========================================

	if (chargeEffect_) {

		chargeEffect_->Update(worldTransform_.translation_, isCharging, isFullCharge);
	}


	float moveX = worldTransform_.translation_.x - prePlayerX;
	TireUpdate(moveX);

	//
	//
	//Debug();
}

void Player::MovementInput() {

	//if (onGround_) {
	//
	//	if (velocity_.y > 0.0f) {
	//		onGround_ = false;
	//	}

	//	KamataEngine::Vector3 acceleration = {};

	//	//右移動
	//	if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT)) {

	//		if (velocity_.x < 0.0f) {
	//			velocity_.x *= (1.0f - kAcceleration);
	//		}

	//		acceleration.x += kAcceleration;

	//		if (lrDirection_ != LRDirection::kRight) {
	//			lrDirection_ = LRDirection::kRight;
	//			turnFirstRotationY_ = worldTransform_.rotation_.y;
	//			turnTimer_ = 0.0f;
	//		}

	//	}
	//	
	//	//左移動
	//	if (KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {

	//		if (velocity_.x > 0.0f) {
	//			velocity_.x *= (1.0f - kAcceleration);
	//		}

	//		acceleration.x -= kAcceleration;

	//		if (lrDirection_ != LRDirection::kLeft) {
	//			lrDirection_ = LRDirection::kLeft;
	//			turnFirstRotationY_ = worldTransform_.rotation_.y;
	//			turnTimer_ =0.0f;
	//		}
	//	}

	//	//数値計算
	//	velocity_.x += acceleration.x;
	//	velocity_.y += acceleration.y;
	//	velocity_.z += acceleration.z;

	//	velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

	//	//上移動ジャンプ
	//	if (KamataEngine::Input::GetInstance()->PushKey(DIK_UP)) {
	//		velocity_.y += kJumpAcceleration;
	//	}

	//	/*//スピード低下
	//	if (!KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT) && !KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT)) {
	//		velocity_.x *= (1.0f - kAcceleration);
	//	}*/

	//} else {
	//	velocity_.y += KamataEngine::Vector3(0, -kGravityAcceleration, 0).y;
	//	velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);

	//	/* if (info.isLanding) {
	//		worldTransform_.translation_.y = 1.0f;
	//		velocity_.x *= (1.0f - kAttenuation);
	//		velocity_.y = 0.0f;
	//		onGround_ = true;
	//	}*/



	//}

	//if (velocity_.y > kLimitJumpSpeed) {

	//	velocity_.y = kLimitJumpSpeed;
	//}
	// ==================================================
	// 地上と空中で操作感を変更
	// ==================================================

	float moveAcceleration = kAcceleration;
	float moveLimitSpeed = kLimitRunSpeed;
	float moveDeceleration = kAcceleration;

	if (!onGround_) {

		moveAcceleration = kAirAcceleration;
		moveLimitSpeed = kAirLimitRunSpeed;
		moveDeceleration = kAirDeceleration;
	}

	// ==================================================
	// 右
	// ==================================================

	if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT)) {

		if (velocity_.x < 0.0f) {
			velocity_.x *= (1.0f - moveDeceleration);
		}

		velocity_.x += moveAcceleration;

		if (lrDirection_ != LRDirection::kRight) {

			lrDirection_ = LRDirection::kRight;

			turnFirstRotationY_ = worldTransform_.rotation_.y;

			turnTimer_ = 0.0f;
		}
	}

	// ==================================================
	// 左
	// ==================================================

	else if (KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {

		if (velocity_.x > 0.0f) {
			velocity_.x *= (1.0f - moveDeceleration);
		}

		velocity_.x -= moveAcceleration;

		if (lrDirection_ != LRDirection::kLeft) {

			lrDirection_ = LRDirection::kLeft;

			turnFirstRotationY_ = worldTransform_.rotation_.y;

			turnTimer_ = 0.0f;
		}
	}

	// ==================================================
	// 左右入力なし
	// ==================================================

	else {

		velocity_.x *= (1.0f - moveDeceleration);

		if (std::abs(velocity_.x) < 0.01f) {
			velocity_.x = 0.0f;
		}
	}

	velocity_.x = std::clamp(velocity_.x, -moveLimitSpeed, moveLimitSpeed);

	// ==================================================
	// ジャンプ
	// ==================================================

	bool jumpStart = false;

	if (onGround_ && KamataEngine::Input::GetInstance()->TriggerKey(DIK_UP)) {

		velocity_.y = kJumpAcceleration;

		// ★押した瞬間に空中状態へ
		onGround_ = false;

		jumpStart = true;
	}

	// ==================================================
	// 重力
	// ==================================================

	if (!onGround_ && !jumpStart) {

		velocity_.y -= kGravityAcceleration;

		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

	if (velocity_.y > kLimitJumpSpeed) {

		velocity_.y = kLimitJumpSpeed;
	}

}

void Player::MapCollisionDetection(CollisionMaPInfo& info) { 

	//上下右左の衝突判定関数
	MCDUpDirection(info);
	MCDDownDirection(info);
	MCDRightDirection(info);
	MCDLeftDirection(info);


}

void Player::JudgmentMovement(const CollisionMaPInfo& info) { 
	worldTransform_.translation_ = Vector3Add(worldTransform_.translation_, info.moveAmount);
	

}

void Player::CeilingContactDetection(const CollisionMaPInfo& info) {
	if (info.isCeilingCollision) {
		KamataEngine::DebugText::GetInstance()->ConsolePrintf("hit ceiling");
		velocity_.y = 0;
	
	}

}

void Player::WallDetection(const CollisionMaPInfo& info) {

	if (info.isWallContact) {
		velocity_.x *= (1.0f - kAttenuationWall);

	}

}

void Player::GroundStateSwitching(const CollisionMaPInfo& info) {
	//if (onGround_) {
	//	if (velocity_.y >= 0.0f) {
	//		onGround_ = false;

	//	} else {
	//		
	//		std::array<KamataEngine::Vector3, kNumCorner> posNow;

	//		for (uint32_t i = 0; i < posNow.size(); i++) {
	//			posNow[i] = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
	//		}

	//		MapChipType mapChipType;

	//		bool hit = false;

	//		MapChipField::IndexSet indexSet;
	//		indexSet = mapChipField_->GetMapChipIndexSetByPos(Vector3Add(posNow[kLeftTop], {0, -kAttenuationLanding, 0}));
	//		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	//		if (mapChipType == MapChipType::kBlock) {
	//			hit = true;
	//		}

	//		indexSet = mapChipField_->GetMapChipIndexSetByPos(Vector3Add(posNow[kRightTop], {0, -kAttenuationLanding, 0}));
	//		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	//		if (mapChipType == MapChipType::kBlock) {
	//			hit = true;
	//		}

	//		if (!hit) {
	//			onGround_ = false;
	//		}
	//	
	//	}
	//
	//} else {

	//	if (info.isLanding) {
	//		onGround_ = true;
	//		velocity_.x *= (1.0f - kAttenuationLanding);
	//		velocity_.y = 0.0f;
	//	}
	//
	//}


		if (onGround_) {

			// =========================================
			// 上昇した時だけ地面から離れる
			// =========================================

			if (velocity_.y > 0.0f) {

				onGround_ = false;

			} else {

				std::array<KamataEngine::Vector3, kNumCorner> posNow;

				for (uint32_t i = 0; i < posNow.size(); i++) {

					posNow[i] = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
				}

				MapChipType mapChipType;

				bool hit = false;

				MapChipField::IndexSet indexSet;

				// =====================================
				// 左足元
				// =====================================

				indexSet = mapChipField_->GetMapChipIndexSetByPos(Vector3Add(posNow[kLeftBottom], {0, -kAttenuationLanding, 0}));

				mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

				if (mapChipType == MapChipType::kBlock) {

					hit = true;
				}

				// =====================================
				// 右足元
				// =====================================

				indexSet = mapChipField_->GetMapChipIndexSetByPos(Vector3Add(posNow[kRightBottom], {0, -kAttenuationLanding, 0}));

				mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

				if (mapChipType == MapChipType::kBlock) {

					hit = true;
				}

				// 足元に床がなければ落下
				if (!hit) {

					onGround_ = false;
				}
			}
		}

		else {

			// =========================================
			// 落下中に床へ接触
			// =========================================

			if (info.isLanding) {

				onGround_ = true;

				velocity_.x *= (1.0f - kAttenuationLanding);

				velocity_.y = 0.0f;
			}
		}

}

void Player::TurningControl() {

	// 左右イージング
	if (turnTimer_ < kTimeTurn) {
		turnTimer_ += 1.0f / 60.0f;

		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> / 2.0f,
		    std::numbers::pi_v<float> * 3.0f / 2.0f,

		};

		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		worldTransform_.rotation_.y = EaseInSine(turnFirstRotationY_, destinationRotationY, turnTimer_, kTimeTurn);
		

	}

}

void Player::MCDUpDirection(CollisionMaPInfo& info) {

	if (info.moveAmount.y <= 0.0f) {
		return;
	}

	//移動後角4つの座標計算
	std::array<KamataEngine::Vector3, kNumCorner> posNew;

	for (uint32_t i = 0; i < posNew.size(); i++) {
		posNew[i] = CornerPosition(Vector3Add(worldTransform_.translation_,info.moveAmount),static_cast<Corner>(i));
	
	}

		// 現在位置
	std::array<KamataEngine::Vector3, kNumCorner> posNow;

	for (uint32_t i = 0; i < posNow.size(); i++) {
		posNow[i] = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	bool hit = false;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPos(posNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPos(posNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}


	if (hit) {

		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPos(posNow[kLeftTop]);

		if (indexSetNow.yIndex != indexSet.yIndex) {

			indexSet = mapChipField_->GetMapChipIndexSetByPos(posNew[kRightTop]);
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

			info.moveAmount.y = std::max(0.0f, (rect.top - worldTransform_.translation_.y) - (0.5f + kBlank));
			info.isCeilingCollision = true;
		}
	}

}

void Player::MCDDownDirection(CollisionMaPInfo& info) {

	if (info.moveAmount.y >= 0.0f) {
		return;
	}

	// 移動後角4つの座標計算
	std::array<KamataEngine::Vector3, kNumCorner> posNew;

	for (uint32_t i = 0; i < posNew.size(); i++) {
		posNew[i] = CornerPosition(Vector3Add(worldTransform_.translation_, info.moveAmount), static_cast<Corner>(i));
	}

	// 現在位置
	std::array<KamataEngine::Vector3, kNumCorner> posNow;

	for (uint32_t i = 0; i < posNow.size(); i++) {
		posNow[i] = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	bool hit = false;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPos(posNew[kLeftBottom]);

	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex -1 );
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPos(posNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	
	if (hit) {

		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPos(posNow[kLeftBottom]);

		if (indexSetNow.yIndex != indexSet.yIndex) {

			indexSet = mapChipField_->GetMapChipIndexSetByPos(posNew[kRightBottom]);
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

			info.moveAmount.y = std::min(0.0f, (rect.bottom - worldTransform_.translation_.y) + (0.5f + kBlank));
			info.isLanding = true;

		}
	}
}

void Player::MCDRightDirection(CollisionMaPInfo& info) {

	if (info.moveAmount.x <= 0.0f) {
		return;
	}

	// 移動後角4つの座標計算
	std::array<KamataEngine::Vector3, kNumCorner> posNew;

	for (uint32_t i = 0; i < posNew.size(); i++) {
		posNew[i] = CornerPosition(Vector3Add(worldTransform_.translation_, info.moveAmount), static_cast<Corner>(i));
	}

		// 現在位置
	std::array<KamataEngine::Vector3, kNumCorner> posNow;

	for (uint32_t i = 0; i < posNow.size(); i++) {
		posNow[i] = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	bool hit = false;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPos(posNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex );
	if (mapChipType == MapChipType::kBlock ) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPos(posNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {

		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPos(posNow[kRightBottom]);

		if (indexSetNow.xIndex != indexSet.xIndex) {

			indexSet = mapChipField_->GetMapChipIndexSetByPos(posNew[kRightBottom]);
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

			info.moveAmount.x = std::min(0.0f, (rect.left - worldTransform_.translation_.x) - (0.5f + kBlank));
			info.isWallContact = true;
		}
	}

}

void Player::MCDLeftDirection(CollisionMaPInfo& info) {

	if (info.moveAmount.x >= 0.0f) {
		return;
	}

	// 移動後角4つの座標計算
	std::array<KamataEngine::Vector3, kNumCorner> posNew;

	for (uint32_t i = 0; i < posNew.size(); i++) {
		posNew[i] = CornerPosition(Vector3Add(worldTransform_.translation_, info.moveAmount), static_cast<Corner>(i));
	}

	// 現在位置
	std::array<KamataEngine::Vector3, kNumCorner> posNow;

	for (uint32_t i = 0; i < posNow.size(); i++) {
		posNow[i] = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	bool hit = false;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPos(posNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPos(posNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {

		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPos(posNow[kLeftBottom]);

		if (indexSetNow.xIndex != indexSet.xIndex) {

			indexSet = mapChipField_->GetMapChipIndexSetByPos(posNew[kLeftBottom]);
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

			info.moveAmount.x = std::max(0.0f, (rect.right - worldTransform_.translation_.x) + (0.5f + kBlank));
			info.isWallContact = true;
		}
	}
}

void Player::BehaviorRootInitialize() {}

void Player::BehaviorDashInitialize() { 
	dashParameter_ = 0;

}

void Player::BehaviorKnockbackInitialize() { 
	knockbackParameter_ = 0;
	worldTransform_.scale_.z = 1.0f;
	worldTransform_.scale_.y = 1.0f;
	dashPhase_ = DashPhase::kCharge;
	dashParameter_ = 0;

}

void Player::AttackInitialize() {

	//chargeAttackState_ = ChargeAttackState::kAttack;
	attackParameter_ = 0;

}

void Player::BehaviorRootUpdate() {

	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_Z)) {
		behaviorRequest_ = Behavior::kDash;
	}


	// 1移動入力
	MovementInput();

	// 2移動量加味して衝突判定
	CollisionMaPInfo collisionMaPInfo;
	collisionMaPInfo.moveAmount = velocity_;
	MapCollisionDetection(collisionMaPInfo);

	// 3判定結果を反映して移動
	JudgmentMovement(collisionMaPInfo);
	

	// 4天井接触での処理
	CeilingContactDetection(collisionMaPInfo);

	// 5壁に接触してる処理
	WallDetection(collisionMaPInfo);

	// 6接地状態の切り替え処理
	GroundStateSwitching(collisionMaPInfo);

	// 7旋回制御
	TurningControl();

	//8プレイヤー親子部品稼働
	armUpdata();

	// 9行列計算
	VariousMakeAffineMatrix();




}

void Player::BehaviorDashUpdate() {
	dashParameter_++;


	/* if (dashParameter_ >= dashTime) {
		behaviorRequest_ = Behavior::kRoot;
	}
	velocity_.x = 1.0f;*/

	//KamataEngine::Vector3 velocity{};

	if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT)) {

		if (lrDirection_ != LRDirection::kRight) {

			lrDirection_ = LRDirection::kRight;

			turnFirstRotationY_ = worldTransform_.rotation_.y;

			turnTimer_ = 0.0f;
		}
	}

	else if (KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {

		if (lrDirection_ != LRDirection::kLeft) {

			lrDirection_ = LRDirection::kLeft;

			turnFirstRotationY_ = worldTransform_.rotation_.y;

			turnTimer_ = 0.0f;
		}
	}

	//if (onGround_) {

	//	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_UP)) {

	//		velocity_.y = kJumpAcceleration;
	//	}

	//}else {

	//	// ダッシュ中でも重力
	//	velocity_.y -= kGravityAcceleration;

	//	velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	//}

	bool dashJumpStart = false;

	if (onGround_ && KamataEngine::Input::GetInstance()->TriggerKey(DIK_UP)) {

		velocity_.y = kJumpAcceleration;

		onGround_ = false;

		dashJumpStart = true;
	}

	if (!onGround_ && !dashJumpStart) {

		velocity_.y -= kGravityAcceleration;

		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

	// 上昇速度制限
	if (velocity_.y > kLimitJumpSpeed) {

		velocity_.y = kLimitJumpSpeed;
	}


	switch (dashPhase_) {
	case Player::DashPhase::kCharge:
		//float t = static_cast<float>(dashParameter_) / chargeTime;
		worldTransform_.scale_.z = EaseOutSine(1.0f, 0.3f, static_cast<float>(dashParameter_), static_cast<float>(chargeTime));
		worldTransform_.scale_.y = EaseOutSine(1.0f, 1.6f, static_cast<float>(dashParameter_), static_cast<float>(chargeTime));
		if (dashParameter_ >= chargeTime) {
			dashPhase_ = DashPhase::kRush;
			dashParameter_ = 0;
		}

		break;
	case Player::DashPhase::kRush:

		worldTransform_.scale_.z = EaseOutSine(0.1f, 1.3f, static_cast<float>(dashParameter_), static_cast<float>(rushTime));
		worldTransform_.scale_.y = EaseInSine(1.6f, 0.7f, static_cast<float>(dashParameter_), static_cast<float>(rushTime));

		if (dashParameter_ >= rushTime) {
			dashPhase_ = DashPhase::kLingering;
			dashParameter_ = 0;
		}

		if (lrDirection_ == LRDirection::kRight) {
			//Vector3Add(velocity_, dashVelocity);
			velocity_.x = 0.8f;

		} else {
			//Vector3Sub(velocity_, dashVelocity);
			velocity_.x = -0.8f;
		}

		break;
	case Player::DashPhase::kLingering:

		worldTransform_.scale_.z = EaseOutSine(1.3f, 1.0f, static_cast<float>(dashParameter_), static_cast<float>(lingeringTime));
		worldTransform_.scale_.y = EaseOutSine(0.7f, 1.0f, static_cast<float>(dashParameter_), static_cast<float>(lingeringTime));

		if (dashParameter_ >= lingeringTime) {
			worldTransform_.scale_.z = 1.0f;
			worldTransform_.scale_.y = 1.0f;

			dashPhase_ = DashPhase::kCharge;
			dashParameter_ = 0;
			behaviorRequest_ = Behavior::kRoot;
		}
		
		break;
	default:
		break;
	}


	// 1移動入力
	//MovementInput();

	// 2移動量加味して衝突判定
	CollisionMaPInfo collisionMaPInfo;
	collisionMaPInfo.moveAmount = velocity_;	
	MapCollisionDetection(collisionMaPInfo);

	// 3判定結果を反映して移動
	JudgmentMovement(collisionMaPInfo);
	

	// 4天井接触での処理
	CeilingContactDetection(collisionMaPInfo);

	// 5壁に接触してる処理
	WallDetection(collisionMaPInfo);

	// 6接地状態の切り替え処理
	GroundStateSwitching(collisionMaPInfo);

	// 7旋回制御
	TurningControl();

	// 8プレイヤー親子部品稼働
	armUpdata();

	// 9行列計算
	VariousMakeAffineMatrix();


	//9攻撃エフェクト行列計算
	worldTransformDash_.translation_ = worldTransform_.translation_;
	worldTransformDash_.rotation_ = worldTransform_.rotation_;
	MakeAffineMatrix(&worldTransformDash_);


}

void Player::BehaviorKnockbackUpdate() {
	knockbackParameter_ ++;

	if (lrDirection_ == LRDirection::kRight) {
		// Vector3Add(velocity_, dashVelocity);
		velocity_.x = -0.8f;

	} else {
		// Vector3Sub(velocity_, dashVelocity);
		velocity_.x = 0.8f;
	}

	if (knockbackParameter_ >= chargeTime ) {

		knockbackParameter_ = 0;
		behaviorRequest_ = Behavior::kRoot;
		BehaviorKnockbackInitialize();
	}

		// 1移動入力
	// MovementInput();

	// 2移動量加味して衝突判定
	CollisionMaPInfo collisionMaPInfo;
	collisionMaPInfo.moveAmount = velocity_;
	// collisionMaPInfo.moveAmount = velocity_;
	MapCollisionDetection(collisionMaPInfo);

	// 3判定結果を反映して移動
	JudgmentMovement(collisionMaPInfo);
	

	// 4天井接触での処理
	CeilingContactDetection(collisionMaPInfo);

	// 5壁に接触してる処理
	WallDetection(collisionMaPInfo);

	// 6接地状態の切り替え処理
	GroundStateSwitching(collisionMaPInfo);

	// 7旋回制御
	TurningControl();

	// 8プレイヤー親子部品稼働
	armUpdata();

	// 9行列計算
	VariousMakeAffineMatrix();

	// 10攻撃エフェクト行列計算
	worldTransformDash_.translation_ = worldTransform_.translation_;
	worldTransformDash_.rotation_ = worldTransform_.rotation_;
	MakeAffineMatrix(&worldTransformDash_);


}

void Player::AttackUpdate() {

	bool attackKey = KamataEngine::Input::GetInstance()->PushKey(DIK_X);

	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_X)) {
		AttackInitialize();

		if (armRightState == ArmState::kIdle) {
			armRightState = ArmState::kCharge;
		
		}else if (armLeftState == ArmState::kIdle) {
			armLeftState = ArmState::kCharge;

		}

	}

	if (KamataEngine::Input::GetInstance()->PushKey(DIK_X)) {
		attackHoldFrame_++;

		if (attackHoldFrame_ > kChargeAttackFrame) {

			if (armRightState == ArmState::kIdle) {
				armRightState = ArmState::kCharge;
			}
			
			if (armLeftState == ArmState::kIdle) {
				armLeftState = ArmState::kCharge;
			}
		}

	}

	//リリース処理が無いので代用
	if (preAttackKey_ && !attackKey) {
		
		//単押し
		if (attackHoldFrame_ < kPunchAttackFrame) {

			AttackInitialize();

			if (armRightState == ArmState::kCharge) {
				rightArmAttackID_ = ++attackIDCounter_;

				armRightState = ArmState::kAttack;

				NormalAttackInitialize(worldTransformRightArm_,rightArmAttackStartPos_,rightArmAttackTargetPos_,rightArmParameter_);
			} 
			
			if (armLeftState == ArmState::kCharge) {
				leftArmAttackID_ = ++attackIDCounter_;
				armLeftState = ArmState::kAttack;

				NormalAttackInitialize(worldTransformLeftArm_, leftArmAttackStartPos_, leftArmAttackTargetPos_, leftArmParameter_);
			}

		}



		//途中チャージ
		if (attackHoldFrame_ >= kPunchAttackFrame && attackHoldFrame_ < kChargeAttackFrame) {
		
			AttackInitialize();

			if (armRightState == ArmState::kCharge) {
				rightArmAttackID_ = ++attackIDCounter_;
				rightChargeShotAttackID_ = ++attackIDCounter_;
				rightChargeShotParameter_ = 0;
				isRightChargeShot = true;

				armRightState = ArmState::kAttack;
				NormalAttackInitialize(worldTransformRightArm_, rightArmAttackStartPos_, rightArmAttackTargetPos_, rightArmParameter_);


			} else if (armLeftState == ArmState::kCharge) {
				leftArmAttackID_ = ++attackIDCounter_;
				leftChargeShotAttackID_ = ++attackIDCounter_;

				leftChargeShotParameter_ = 0;
				isLeftChargeShot = true;
				armLeftState = ArmState::kAttack;
				NormalAttackInitialize(worldTransformLeftArm_, leftArmAttackStartPos_, leftArmAttackTargetPos_, leftArmParameter_);

			}

			worldTransformLeftChargeShot_.translation_ = worldTransformLeftArm_.translation_;
			worldTransformRightChargeShot_.translation_ = worldTransformRightArm_.translation_;

			lRDLeftChargeShot = lrDirection_;
			lRDRightChargeShot = lrDirection_;
			
		}

			//フルチャージ
		if (attackHoldFrame_ >= kChargeAttackFrame) {
			AttackInitialize();

			rightArmAttackID_ = ++attackIDCounter_;
			leftArmAttackID_ = ++attackIDCounter_;

			//NormalAttackInitialize(worldTransformRightArm_, rightArmAttackStartPos_, rightArmAttackTargetPos_, rightArmParameter_);
			//NormalAttackInitialize(worldTransformLeftArm_, leftArmAttackStartPos_, leftArmAttackTargetPos_, leftArmParameter_);
			ChargeAttackInitialize(worldTransformRightArm_, rightArmCAAC, rightArmAttackStartPos_, rightArmReturnStartPos_, rightArmParameter_);
			ChargeAttackInitialize(worldTransformLeftArm_, leftArmCAAC, leftArmAttackStartPos_, leftArmReturnStartPos_, leftArmParameter_);
			armRightState = ArmState::kChargeAttack;
			armLeftState = ArmState::kChargeAttack;
		}

		attackHoldFrame_ = 0;
	}

	RightArmAttackUpdate();
	LeftArmAttackUpdate();
	ChargeShotUpdate();

	MakeAffineMatrix(&worldTransformLeftChargeShot_);
	MakeAffineMatrix(&worldTransformRightChargeShot_);

	preAttackKey_ = attackKey;

}

void Player::RightArmAttackUpdate() {

	float armAttackRotZ = std::numbers::pi_v<float> / 2.0f;

	if (lrDirection_ == LRDirection::kLeft) {
		armAttackRotZ *= -1.0f;
	}

	switch (armRightState) {

	case Player::ArmState::kIdle:

		worldTransformRightArm_.translation_ = GetRightArmPos();

		worldTransformRightArm_.rotation_ = worldTransform_.rotation_;

		break;


	case Player::ArmState::kCharge:

		worldTransformRightArm_.translation_ = GetRightArmPos();

		worldTransformRightArm_.rotation_ = worldTransform_.rotation_;

		worldTransformRightArm_.rotation_.z += armAttackRotZ;

		break;
	case Player::ArmState::kAttack:
		NormalAttackMove(worldTransformRightArm_, rightArmAttackStartPos_, rightArmAttackTargetPos_, rightArmReturnStartPos_,rightArmParameter_, armRightState);

		break;

	case Player::ArmState::kChargeAttack:
		ChargeAttackMove(worldTransformRightArm_, rightArmCAAC, rightArmAttackStartPos_, rightArmReturnStartPos_, rightArmParameter_,armRightState);

		break;

	case Player::ArmState::kReturn:

		ArmReturnMove(worldTransformRightArm_, rightArmReturnStartPos_, GetRightArmPos() ,rightArmParameter_, armRightState);
		break;

	}

}

void Player::LeftArmAttackUpdate() {

		float armAttackRotZ = std::numbers::pi_v<float> / 2.0f;

	if (lrDirection_ == LRDirection::kLeft) {
	    armAttackRotZ *= -1.0f;
    }


	switch (armLeftState) {

	case Player::ArmState::kIdle:
		worldTransformLeftArm_.translation_ = GetLeftArmPos();

		worldTransformLeftArm_.rotation_ = worldTransform_.rotation_;

		break;

	case Player::ArmState::kCharge:

		worldTransformLeftArm_.translation_ = GetLeftArmPos();

		worldTransformLeftArm_.rotation_ = worldTransform_.rotation_;

		worldTransformLeftArm_.rotation_.z += armAttackRotZ;
		break;

	case Player::ArmState::kAttack:
		NormalAttackMove(worldTransformLeftArm_, leftArmAttackStartPos_, leftArmAttackTargetPos_, leftArmReturnStartPos_, leftArmParameter_, armLeftState);
		break;

	case Player::ArmState::kChargeAttack:
		ChargeAttackMove(worldTransformLeftArm_, leftArmCAAC, leftArmAttackStartPos_, leftArmReturnStartPos_, leftArmParameter_,armLeftState);
		break;

	case Player::ArmState::kReturn:
		ArmReturnMove(worldTransformLeftArm_, leftArmReturnStartPos_, GetLeftArmPos(), leftArmParameter_, armLeftState);
		break;

	}

}

void Player::NormalAttackInitialize(KamataEngine::WorldTransform& armTransform, KamataEngine::Vector3& startPos, KamataEngine::Vector3& targetPos, uint32_t& parameter) {
	
	parameter = 0;
	startPos = armTransform.translation_;

	float directionX = 1.0f;

	if (lrDirection_ == LRDirection::kLeft) {
		directionX *= -1.0f;
	}

	targetPos = startPos;

	targetPos.x += directionX * kArmAttackDistance;

}

void Player::NormalAttackMove(
    KamataEngine::WorldTransform& armTransform, const KamataEngine::Vector3& startPos, const KamataEngine::Vector3& targetPos, KamataEngine::Vector3& returnStartPos, uint32_t& parameter,
    ArmState& state) { 
	parameter++;

	armTransform.translation_.x = EaseOutQuint(startPos.x, targetPos.x, float(parameter), float(kArmAttackTime));
	armTransform.translation_.y = EaseOutQuint(startPos.y, targetPos.y, float(parameter), float(kArmAttackTime));
	armTransform.translation_.z = EaseOutQuint(startPos.z, targetPos.z, float(parameter), float(kArmAttackTime));


	if (parameter >= kArmAttackTime) {
		armTransform.translation_ = targetPos;
		ArmReturnInitialize(armTransform, returnStartPos, parameter);
		state = ArmState::kReturn;

	}

}

void Player::ChargeAttackMove(KamataEngine::WorldTransform& armTransform, ChargeAttackArmConfig& CAAC, KamataEngine::Vector3& startPos, KamataEngine::Vector3& returnStartPos, uint32_t& parameter,ArmState& state) {
	
	switch (CAAC.chargeAttackPhase) {
	case Player::ChargeAttackPhase::kPrepare:
		parameter++;
		armTransform.translation_.x = EaseOutQuint(startPos.x, CAAC.preparePos.x, float(parameter), float(CAAC.kPrepareTime));
		armTransform.translation_.y = EaseOutQuint(startPos.y, CAAC.preparePos.y, float(parameter), float(CAAC.kPrepareTime));
		armTransform.translation_.z = EaseOutQuint(startPos.z, CAAC.preparePos.z, float(parameter), float(CAAC.kPrepareTime));
			
		armTransform.rotation_.z = EaseOutQuint(CAAC.startRot, CAAC.prepareRot, float(parameter), float(CAAC.kPrepareTime));

		if (parameter >= CAAC.kPrepareTime) {
			parameter = 0;
			armTransform.translation_ = CAAC.preparePos;
			CAAC.chargeAttackPhase = ChargeAttackPhase::kFall;
		}

		break;
	case Player::ChargeAttackPhase::kFall:
		parameter++;

		armTransform.translation_.x = EaseOutSine(CAAC.preparePos.x, CAAC.fallPos.x, float(parameter), float(CAAC.kFallTime));
		armTransform.translation_.y = EaseOutSine(CAAC.preparePos.y, CAAC.fallPos.y, float(parameter), float(CAAC.kFallTime));
		armTransform.translation_.z = EaseOutSine(CAAC.preparePos.z, CAAC.fallPos.z, float(parameter), float(CAAC.kFallTime));

		if (parameter >= CAAC.kFallTime) {
			parameter = 0;
			armTransform.translation_ = CAAC.fallPos;
			CAAC.chargeAttackPhase = ChargeAttackPhase::kImpact;
		}

		break;
	case Player::ChargeAttackPhase::kImpact:
		parameter++;

		if (parameter >= CAAC.kImpactTime) {
			parameter = 0;
			ArmReturnInitialize(armTransform,returnStartPos,parameter);
			state = ArmState::kReturn;
			CAAC.chargeAttackPhase = ChargeAttackPhase::kPrepare;

		}

		break;

	}

}

void Player::ChargeAttackInitialize(KamataEngine::WorldTransform& armTransform, ChargeAttackArmConfig& CAAC, KamataEngine::Vector3& startPos, KamataEngine::Vector3& returnStartPos, uint32_t& parameter) { 
	
	parameter = 0;
	startPos = armTransform.translation_;
	CAAC.preparePos = armTransform.translation_;
	CAAC.fallPos = armTransform.translation_;

	CAAC.preparePos.y += 5.0f;

	if (lrDirection_ == LRDirection::kRight) {
		CAAC.preparePos.x += 5.0f;
		CAAC.fallPos.x += 5.0f;
	}

	if (lrDirection_ == LRDirection::kLeft) {
		CAAC.preparePos.x -= 5.0f;
		CAAC.fallPos.x -= 5.0f;
	}

	CAAC.startRot = armTransform.rotation_.z;
	returnStartPos = CAAC.fallPos;

	CAAC.chargeAttackPhase = ChargeAttackPhase::kPrepare;

}

void Player::ArmReturnInitialize(KamataEngine::WorldTransform& armTransform, KamataEngine::Vector3& startPos, uint32_t& parameter) {
	
	parameter = 0;
	startPos = armTransform.translation_;

}

void Player::ArmReturnMove(KamataEngine::WorldTransform& armTransform, const KamataEngine::Vector3& startPos, const KamataEngine::Vector3& targetPos, uint32_t& parameter, ArmState& state) {
	
	parameter++;

	armTransform.translation_.x = EaseOutElastic(startPos.x, targetPos.x, float(parameter), float(kArmReturnTime));
	armTransform.translation_.y = EaseOutElastic(startPos.y, targetPos.y, float(parameter), float(kArmReturnTime));
	armTransform.translation_.z = EaseOutElastic(startPos.z, targetPos.z, float(parameter), float(kArmReturnTime));

	float armAttackRotZ = std::numbers::pi_v<float> / 2.0f;

	if (lrDirection_ == LRDirection::kLeft) {
		armAttackRotZ *= -1.0f;
	}


	armTransform.rotation_.z = EaseOutElastic(armAttackRotZ, worldTransform_.rotation_.z, float(parameter), float(kArmReturnTime));

	if (parameter >= kArmReturnTime) {
		armTransform.translation_ = targetPos;
		armTransform.rotation_.z = worldTransform_.rotation_.z;
		parameter = 0;
		state = ArmState::kIdle;
	}
}

void Player::armUpdata() {


}

void Player::ChargeShotUpdate() {

	float d = 0.5f;

	if (isLeftChargeShot) {


		//worldTransformLeftChargeShot_.translation_ += worldTransformLeftArm_.translation_;
		worldTransformLeftChargeShot_.rotation_ = worldTransformLeftArm_.rotation_;
		leftChargeShotParameter_++;



		switch (lRDLeftChargeShot) {
		case LRDirection::kRight:

			worldTransformLeftChargeShot_.translation_.x += d;
			worldTransformLeftChargeShot_.rotation_.x += std::numbers::pi_v<float> / 2.0f;

			break;
		case LRDirection::kLeft:

			worldTransformLeftChargeShot_.translation_.x += -d;
			worldTransformLeftChargeShot_.rotation_.x += std::numbers::pi_v<float> /2.0f;
			
			break;

		}

		if (leftChargeShotParameter_ >= leftChargeShotTime) {
			isLeftChargeShot = false;
			leftChargeShotParameter_ = 0;
		}
	}
	

	if (isRightChargeShot) {

		
		//worldTransformRightChargeShot_.translation_ = worldTransformRightArm_.translation_;
		worldTransformRightChargeShot_.rotation_ = worldTransformRightArm_.rotation_;
		// worldTransformChargeShot_.rotation_.x =  std::numbers::pi_v<float> / 2.0f;
		rightChargeShotParameter_++;

		
		switch (lRDRightChargeShot) {
		case LRDirection::kRight:
			worldTransformRightChargeShot_.translation_.x += d;
			worldTransformRightChargeShot_.rotation_.x += std::numbers::pi_v<float> / 2.0f;
			break;
		case LRDirection::kLeft:
			worldTransformRightChargeShot_.translation_.x += -d;
			worldTransformRightChargeShot_.rotation_.x += std::numbers::pi_v<float> / 2.0f;
			break;

		}


		if (rightChargeShotParameter_ >= rightChargeShotTime) {
			isRightChargeShot = false;
			rightChargeShotParameter_ = 0;
		}


	}
}

void Player::TireUpdate(float moveX) {

	    // =====================================
	// プレイヤーについてくる
	// =====================================

	worldTransformTire_.translation_ = worldTransform_.translation_;

	worldTransformTire_.translation_.y += kTireOffsetY;

	// =====================================
	// 左右旋回についてくる
	// =====================================

	// Player本体は右向きがY=90度なので、
	// タイヤ側の基本方向を90度戻す
	worldTransformTire_.rotation_.y = worldTransform_.rotation_.y - std::numbers::pi_v<float> / 2.0f;

	// =====================================
	// 走行回転
	// =====================================


    tireRotation_ -= moveX * kTireRotationSpeed;

	worldTransformTire_.rotation_.z = tireRotation_;

	MakeTireAffineMatrix(&worldTransformTire_);
}

KamataEngine::Vector3 Player::GetRightArmPos() {
	float armDistance = 0.7f;

	KamataEngine::Vector3 re = worldTransform_.translation_;

	//re.x += sinf(worldTransform_.rotation_.y + std::numbers::pi_v<float> / 2.0f) * armDistance;

	//re.z += cosf(worldTransform_.rotation_.y + std::numbers::pi_v<float> / 2.0f) * armDistance;
	//  =====================================
	//  腕を少し上へ
	//  =====================================

	re.y += kArmOffsetY;

	// =====================================
	// 通常の左右位置
	// =====================================

	re.x += sinf(worldTransform_.rotation_.y + std::numbers::pi_v<float> / 2.0f) * armDistance;

	re.z += cosf(worldTransform_.rotation_.y + std::numbers::pi_v<float> / 2.0f) * armDistance;

	// =====================================
	// ダッシュ中は腕を後ろへ
	// =====================================

	if (behavior_ == Behavior::kDash) {

		float dashArmOffset = 0.0f;

		switch (dashPhase_) {

		case DashPhase::kCharge:

			// 徐々に後ろへ
			dashArmOffset = EaseOutSine(0.0f, kDashArmBackOffset, static_cast<float>(dashParameter_), static_cast<float>(chargeTime));

			break;

		case DashPhase::kRush:

			// ダッシュ中は後ろでキープ
			dashArmOffset = kDashArmBackOffset;

			break;

		case DashPhase::kLingering:

			// 徐々に元の位置へ
			dashArmOffset = EaseOutSine(kDashArmBackOffset, 0.0f, static_cast<float>(dashParameter_), static_cast<float>(lingeringTime));

			break;
		}

		// 右向きなら左側が「後ろ」
		if (lrDirection_ == LRDirection::kRight) {

			re.x -= dashArmOffset;

		} else {

			re.x += dashArmOffset;
		}
	}

	return re;
}

KamataEngine::Vector3 Player::GetLeftArmPos() { 
    float armDistance = 0.7f;

	KamataEngine::Vector3 re = worldTransform_.translation_;

	//re.x += sinf(worldTransform_.rotation_.y - std::numbers::pi_v<float> / 2.0f) * armDistance;

	//re.z += cosf(worldTransform_.rotation_.y - std::numbers::pi_v<float> / 2.0f) * armDistance;

	
	// 腕を少し上へ
	re.y += kArmOffsetY;

	re.x += sinf(worldTransform_.rotation_.y - std::numbers::pi_v<float> / 2.0f) * armDistance;

	re.z += cosf(worldTransform_.rotation_.y - std::numbers::pi_v<float> / 2.0f) * armDistance;

	// =====================================
	// ダッシュ中
	// =====================================

	if (behavior_ == Behavior::kDash) {

		float dashArmOffset = 0.0f;

		switch (dashPhase_) {

		case DashPhase::kCharge:

			dashArmOffset = EaseOutSine(0.0f, kDashArmBackOffset, static_cast<float>(dashParameter_), static_cast<float>(chargeTime));

			break;

		case DashPhase::kRush:

			dashArmOffset = kDashArmBackOffset;

			break;

		case DashPhase::kLingering:

			dashArmOffset = EaseOutSine(kDashArmBackOffset, 0.0f, static_cast<float>(dashParameter_), static_cast<float>(lingeringTime));

			break;
		}

		if (lrDirection_ == LRDirection::kRight) {

			re.x -= dashArmOffset;

		} else {

			re.x += dashArmOffset;
		}
	}

	return re;
}


void Player::Draw() {

	model_->Draw(worldTransform_, *camera_);
	modelTire_->Draw(worldTransformTire_, *camera_);
	modelArm_->Draw(worldTransformRightArm_, *camera_);
	modelArm_->Draw(worldTransformLeftArm_, *camera_);
	
	if (dashPhase_ == DashPhase::kRush) {
		//modelAttack_->Draw(worldTransformDash_, *camera_);
		//modelPlayerAttackEffect_->Draw(worldTransformDash_, *camera_);
	}

	if (isLeftChargeShot) {
		modelAttack_->Draw(worldTransformLeftChargeShot_, *camera_);
	}

	if (isRightChargeShot) {
		modelAttack_->Draw(worldTransformRightChargeShot_, *camera_);
	}


	if (chargeEffect_) {

		chargeEffect_->Draw();
	}

}

KamataEngine::Vector3 Player::CornerPosition(const KamataEngine::Vector3& center, Corner corner) { 

	KamataEngine::Vector3 offsetTable[kNumCorner] = {
	    {kWidth / 2.0f, -kHeigth / 2.0f, 0},
        {-kWidth / 2.0f, -kHeigth / 2.0f, 0},
        {kWidth / 2.0f,  kHeigth / 2.0f,  0},
        {-kWidth / 2.0f, kHeigth / 2.0f,  0},
	};

	return Vector3Add(center, offsetTable[static_cast<uint32_t>(corner)]);


	/* if (corner == kRightBottom) {

		KamataEngine::Vector3 a = {kWidth / 2.0f, -kHeigth / 2.0f, 0};
		return Vector3Add(center,a);
	
	} else if (corner == kRightBottom) {
		
		KamataEngine::Vector3 a = {-kWidth / 2.0f, -kHeigth / 2.0f, 0};
		return Vector3Add(center, a);

	} else if (corner == kRightBottom) {

		KamataEngine::Vector3 a = {kWidth / 2.0f, kHeigth / 2.0f, 0};
		return Vector3Add(center, a);

	} else if (corner == kRightBottom) {

		KamataEngine::Vector3 a = {-kWidth / 2.0f, kHeigth / 2.0f, 0};
		return Vector3Add(center, a);

	}*/

}

const KamataEngine::WorldTransform& Player::GetWorldTransform() {
	return worldTransform_;  

}

KamataEngine::Vector3 Player::GetWorldPos() {

	KamataEngine::Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB Player::GetAABB() { 
	KamataEngine::Vector3 worldPos = GetWorldPos();
	AABB aabb;
	aabb.min = {worldPos.x - kWidth, worldPos.y - kHeigth, worldPos.z - kWidth};
	aabb.max = {worldPos.x + kWidth, worldPos.y + kHeigth, worldPos.z + kWidth};

	return aabb;
}

AABB Player::GetRightArmAttackAABB() { 
	KamataEngine::Vector3 worldPos = worldTransformRightArm_.translation_;
	AABB aabb;
	aabb.min = {worldPos.x - kWidth, worldPos.y - kHeigth, worldPos.z - kWidth};
	aabb.max = {worldPos.x + kWidth, worldPos.y + kHeigth, worldPos.z + kWidth};

	return aabb;
}

AABB Player::GetLeftArmAttackAABB() { 
	KamataEngine::Vector3 worldPos = worldTransformLeftArm_.translation_;
	AABB aabb;
	aabb.min = {worldPos.x - kWidth, worldPos.y - kHeigth, worldPos.z - kWidth};
	aabb.max = {worldPos.x + kWidth, worldPos.y + kHeigth, worldPos.z + kWidth};

	return aabb;
}

AABB Player::GetRightChargeShotAABB() {
	KamataEngine::Vector3 worldPos = worldTransformRightChargeShot_.translation_;
	AABB aabb;
	aabb.min = {worldPos.x - kWidth, worldPos.y - kHeigth, worldPos.z - kWidth};
	aabb.max = {worldPos.x + kWidth, worldPos.y + kHeigth, worldPos.z + kWidth};

	return aabb;
}

AABB Player::GetLeftChargeShotAABB() {
	KamataEngine::Vector3 worldPos = worldTransformLeftChargeShot_.translation_;
	AABB aabb;
	aabb.min = {worldPos.x - kWidth, worldPos.y - kHeigth, worldPos.z - kWidth};
	aabb.max = {worldPos.x + kWidth, worldPos.y + kHeigth, worldPos.z + kWidth};

	return aabb;
}

bool Player::IsRightArmAttacking() const { 
	
    // 通常パンチ
	if (armRightState == ArmState::kAttack) {
		return true;
	}

	// フルチャージ
	if (armRightState == ArmState::kChargeAttack) {

		// 落下中か着弾中だけ攻撃判定
		if (rightArmCAAC.chargeAttackPhase == ChargeAttackPhase::kFall || rightArmCAAC.chargeAttackPhase == ChargeAttackPhase::kImpact) {

			return true;
		}
	}

	return false;
}

bool Player::IsLeftArmAttacking() const {

	// 通常パンチ
	if (armLeftState == ArmState::kAttack) {
		return true;
	}

	// フルチャージ
	if (armLeftState == ArmState::kChargeAttack) {

		if (leftArmCAAC.chargeAttackPhase == ChargeAttackPhase::kFall || leftArmCAAC.chargeAttackPhase == ChargeAttackPhase::kImpact) {

			return true;
		}
	}

	return false;
}



void Player::DisableRightChargeShot() { 
	isRightChargeShot = true;
	rightChargeShotParameter_ = 0;

}

void Player::DisableLeftChargeShot() {
	isLeftChargeShot = true;
	leftChargeShotParameter_ = 0;


}

void Player::SetMapChipField(MapChipField* mapChipField) { 
	mapChipField_ = mapChipField; 
}

void Player::OnCollision(const Enemy* enemy) {
	if (isDash()) {
		return;
	}

	(void)enemy;
	isDead_ = true;
}

void Player::OnShieldCollision(const ShieldEnemy* shieldEnemy) {
	if (isDash()) {
		return;
	}

	(void)shieldEnemy;
	isDead_ = true;
}

void Player::OnEnemyCollision() { 
	isDead_ = true;

}

void Player::VariousMakeAffineMatrix() {

	MakeAffineMatrix(&worldTransformRightArm_);
	MakeAffineMatrix(&worldTransformLeftArm_);
	MakeAffineMatrix(&worldTransform_);
	MakeAffineMatrix(&worldTransformLeftChargeShot_);
	MakeAffineMatrix(&worldTransformRightChargeShot_);

}

void Player::MakeTireAffineMatrix(KamataEngine::WorldTransform* worldTransform) {

	KamataEngine::Matrix4x4 MatrixInitialize = {};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			MatrixInitialize.m[i][j] = 0.0f;
		}
	}

	for (int i = 0; i < 4; i++) {
		MatrixInitialize.m[i][i] = 1.0f;
	}

	KamataEngine::Matrix4x4 afin = MatrixInitialize;

	// ==============================
	// Scale
	// ==============================

	KamataEngine::Matrix4x4 S = MatrixInitialize;

	S.m[0][0] = worldTransform->scale_.x;
	S.m[1][1] = worldTransform->scale_.y;
	S.m[2][2] = worldTransform->scale_.z;

	// ==============================
	// Translation
	// ==============================

	KamataEngine::Matrix4x4 T = MatrixInitialize;

	T.m[3][0] = worldTransform->translation_.x;
	T.m[3][1] = worldTransform->translation_.y;
	T.m[3][2] = worldTransform->translation_.z;

	// ==============================
	// X Rotation
	// ==============================

	KamataEngine::Matrix4x4 Rx = MatrixInitialize;

	Rx.m[0][0] = 1.0f;
	Rx.m[1][1] = cosf(worldTransform->rotation_.x);
	Rx.m[2][2] = cosf(worldTransform->rotation_.x);

	Rx.m[1][2] = sinf(worldTransform->rotation_.x);
	Rx.m[2][1] = -sinf(worldTransform->rotation_.x);

	// ==============================
	// Y Rotation
	// ==============================

	KamataEngine::Matrix4x4 Ry = MatrixInitialize;

	Ry.m[1][1] = 1.0f;
	Ry.m[0][0] = cosf(worldTransform->rotation_.y);
	Ry.m[2][2] = cosf(worldTransform->rotation_.y);

	Ry.m[2][0] = sinf(worldTransform->rotation_.y);
	Ry.m[0][2] = -sinf(worldTransform->rotation_.y);

	// ==============================
	// Z Rotation
	// ==============================

	KamataEngine::Matrix4x4 Rz = MatrixInitialize;

	Rz.m[2][2] = 1.0f;
	Rz.m[0][0] = cosf(worldTransform->rotation_.z);
	Rz.m[1][1] = cosf(worldTransform->rotation_.z);

	Rz.m[1][0] = -sinf(worldTransform->rotation_.z);
	Rz.m[0][1] = sinf(worldTransform->rotation_.z);

	// ==============================
	// 回転合成
	// ==============================

	KamataEngine::Matrix4x4 R = MatrixInitialize;

	// 通常版
	// R = Multiply(Rx, Multiply(Ry, Rz));

	// ★タイヤ専用
	// Zで転がしてから、Yでプレイヤーと一緒に振り向く
	R = Multiply(Rx, Multiply(Rz, Ry));

	// ==============================
	// Affine
	// ==============================

	afin = Multiply(S, Multiply(R, T));

	worldTransform->matWorld_ = afin;

	worldTransform->TransferMatrix();

}

void Player::Debug() {
#ifdef _DEBUG

	ImGui::Text("Right Arm State");

	switch (armRightState) {
	case ArmState::kIdle:
		ImGui::Text("Idle");
		break;

	case ArmState::kCharge:
		ImGui::Text("Charge");
		break;


	case ArmState::kAttack:
		ImGui::Text("Attack");
		break;

	case ArmState::kChargeAttack:
		ImGui::Text("ChargeAttack");
		break;

	case ArmState::kReturn:
		ImGui::Text("Return");
		break;
	}

	ImGui::Text("Left Arm State");

	switch (armLeftState) {
	case ArmState::kIdle:
		ImGui::Text("Idle");
		break;

	case ArmState::kCharge:
		ImGui::Text("Charge");
		break;


	case ArmState::kAttack:
		ImGui::Text("Attack");
		break;

	case ArmState::kChargeAttack:
		ImGui::Text("ChargeAttack");
		break;

	case ArmState::kReturn:
		ImGui::Text("Return");
		break;
	}

	ImGui::Text("ChargeAttackPhase");

	switch (leftArmCAAC.chargeAttackPhase) {
	case Player::ChargeAttackPhase::kPrepare:
		ImGui::Text("Prepare");
		break;
	case Player::ChargeAttackPhase::kFall:
		ImGui::Text("Fall");
		break;
	case Player::ChargeAttackPhase::kImpact:
		ImGui::Text("Impact");
		break;
	default:
		break;
	}

	ImGui::Text("lRDLeftChargeShot ");
	switch (lRDLeftChargeShot) {
	case LRDirection::kRight:
		ImGui::Text("Right");
		break;
	case LRDirection::kLeft:
		ImGui::Text("Left");
		break;

	}

		ImGui::Text("lRDRightChargeShot ");
	switch (lRDRightChargeShot) {
	case LRDirection::kRight:
		ImGui::Text("Right");
		break;
	case LRDirection::kLeft:
		ImGui::Text("Left");
		break;
	}

	
	#endif
}

