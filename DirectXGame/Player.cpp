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

}

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera,const KamataEngine::Vector3 pos) { 

	assert(model);
	model_ = model;		
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;
		
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

}

void Player::Update() {
	
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



}

void Player::MovementInput() {

	if (onGround_) {
	
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}

		KamataEngine::Vector3 acceleration = {};

		//右移動
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT)) {

			if (velocity_.x < 0.0f) {
				velocity_.x *= (1.0f - kAcceleration);
			}

			acceleration.x += kAcceleration;

			if (lrDirection_ != LRDirection::kRight) {
				lrDirection_ = LRDirection::kRight;
				turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = kTimeTurn;
			}

		}
		
		//左移動
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {

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

		//数値計算
		velocity_.x += acceleration.x;
		velocity_.y += acceleration.y;
		velocity_.z += acceleration.z;

		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		//上移動ジャンプ
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_.y += kJumpAcceleration;
		}

		/*//スピード低下
		if (!KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT) && !KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT)) {
			velocity_.x *= (1.0f - kAcceleration);
		}*/

	} else {
		velocity_.y += KamataEngine::Vector3(0, -kGravityAcceleration, 0).y;
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);

		/* if (info.isLanding) {
			worldTransform_.translation_.y = 1.0f;
			velocity_.x *= (1.0f - kAttenuation);
			velocity_.y = 0.0f;
			onGround_ = true;
		}*/



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
	if (onGround_) {
		if (velocity_.y >= 0.0f) {
			onGround_ = false;

		} else {
			
			std::array<KamataEngine::Vector3, kNumCorner> posNow;

			for (uint32_t i = 0; i < posNow.size(); i++) {
				posNow[i] = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
			}

			MapChipType mapChipType;

			bool hit = false;

			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPos(Vector3Add(posNow[kLeftTop], {0, -kAttenuationLanding, 0}));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			indexSet = mapChipField_->GetMapChipIndexSetByPos(Vector3Add(posNow[kRightTop], {0, -kAttenuationLanding, 0}));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			if (!hit) {
				onGround_ = false;
			}
		
		}
	
	} else {

		if (info.isLanding) {
			onGround_ = true;
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
		}
	
	}


}

void Player::TurningControl() {

	// 左右イージング
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;

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
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPos(posNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
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
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPos(posNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
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


void Player::Draw() {

	model_->Draw(worldTransform_, *camera_);
	
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



void Player::SetMapChipField(MapChipField* mapChipField) { 
	mapChipField_ = mapChipField; 
}

void Player::OnCollision(const Enemy* enemy) { 
	(void)enemy;
	isDead_ = true;
}


 



