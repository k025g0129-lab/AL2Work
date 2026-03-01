#include "function.h"
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI
#include <numbers>

KamataEngine::Matrix4x4 Multiply(KamataEngine::Matrix4x4 matrix1, KamataEngine::Matrix4x4 matrix2) {

	KamataEngine::Matrix4x4 a;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			a.m[i][j] = 0;
			for (int k = 0; k < 4; k++) {
				a.m[i][j] += matrix1.m[i][k] * matrix2.m[k][j];
			}
		}
	}
	return a;

}

void MakeAffineMatrix(KamataEngine::WorldTransform* worldTransform) {
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

	KamataEngine::Matrix4x4 S = MatrixInitialize;
	S.m[0][0] = worldTransform->scale_.x;
	S.m[1][1] = worldTransform->scale_.y;
	S.m[2][2] = worldTransform->scale_.z;

	KamataEngine::Matrix4x4 T = MatrixInitialize;
	T.m[3][0] = worldTransform->translation_.x;
	T.m[3][1] = worldTransform->translation_.y;
	T.m[3][2] = worldTransform->translation_.z;

	KamataEngine::Matrix4x4 Rx = MatrixInitialize;
	Rx.m[0][0] = 1.0f;
	Rx.m[1][1] = cosf(worldTransform->rotation_.x);
	Rx.m[2][2] = cosf(worldTransform->rotation_.x);
	Rx.m[1][2] = sinf(worldTransform->rotation_.x);
	Rx.m[2][1] = -sinf(worldTransform->rotation_.x);
	KamataEngine::Matrix4x4 Ry = MatrixInitialize;
	Ry.m[1][1] = 1.0f;
	Ry.m[0][0] = cosf(worldTransform->rotation_.y);
	Ry.m[2][2] = cosf(worldTransform->rotation_.y);
	Ry.m[2][0] = sinf(worldTransform->rotation_.y);
	Ry.m[0][2] = -sinf(worldTransform->rotation_.y);
	KamataEngine::Matrix4x4 Rz = MatrixInitialize;
	Rz.m[2][2] = 1.0f;
	Rz.m[0][0] = cosf(worldTransform->rotation_.z);
	Rz.m[1][1] = cosf(worldTransform->rotation_.z);
	Rz.m[1][0] = -sinf(worldTransform->rotation_.z);
	Rz.m[0][1] = sinf(worldTransform->rotation_.z);

	KamataEngine::Matrix4x4 R = MatrixInitialize;
	R = Multiply(Rx, Multiply(Ry, Rz));
	afin = Multiply(S, Multiply(R, T));

	worldTransform->matWorld_ = afin;

	worldTransform->TransferMatrix();


}

float EaseInSine(float self, float target, float t, float maxT) { 
	float tt = 1.0f - (t / maxT);

	float ease = 1.0f - cosf((tt * 3.14f) / 2.0f);

	//return self + (target - self) * ease;
	return self * (1.0f - ease) + target * ease;


}

KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& a, const KamataEngine::Vector3& b, float t) {
	KamataEngine::Vector3 aaa;
	aaa.x = a.x + (b.x - a.x) * t;
	aaa.y = a.y + (b.y - a.y) * t;
	aaa.z = a.z + (b.z - a.z) * t;
	
	return aaa;
	;

}

KamataEngine::Vector3 Vector3Add(KamataEngine::Vector3 a, KamataEngine::Vector3 b) { 
	KamataEngine::Vector3 c;
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;

	return c;

}

float RadToDeg(float radian) {

	return radian * std::numbers::pi_v<float> / 180.0f;

}

int IsCollisionAABB2D(const AABB& a, const AABB& b) {

	if (a.max.x < b.min.x) {
		return false;
	}
	if (a.min.x > b.max.x) {
		return false;
	}

	if (a.max.y < b.min.y) {
		return false;
	}	
	if (a.min.y > b.max.y) {
		return false;
	}

	if (a.max.z < b.min.z) {
		return false;
	}
	if (a.min.z > b.max.z) {
		return false;
	}

	return true;

}

KamataEngine::Matrix4x4 MakeRotateZMatrix(float radian) { 

	 KamataEngine::Matrix4x4 a{};

	float cosSita = cosf(radian);
	float sinSita = sinf(radian);

	a.m[0][0] = cosSita;
	a.m[0][1] = sinSita;
	a.m[0][2] = 0.0f;
	a.m[0][3] = 0.0f;

	a.m[1][0] = -sinSita;
	a.m[1][1] = cosSita;
	a.m[1][2] = 0.0f;
	a.m[1][3] = 0.0f;

	a.m[2][0] = 0.0f;
	a.m[2][1] = 0.0f;
	a.m[2][2] = 1.0f;
	a.m[2][3] = 0.0f;

	a.m[3][0] = 0.0f;
	a.m[3][1] = 0.0f;
	a.m[3][2] = 0.0f;
	a.m[3][3] = 1.0f;

	return a;

}

KamataEngine::Vector3 TransformM4x4ToV3(KamataEngine::Vector3 v, KamataEngine::Matrix4x4 m) { 
	KamataEngine::Vector3 a;

	a.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0];

	a.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1];

	a.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2];

	return a;


}
