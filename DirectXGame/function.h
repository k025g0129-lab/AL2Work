#pragma once
#include "KamataEngine.h"
#include <vector>
#include <random>

struct AABB {
	KamataEngine::Vector3 min;
	KamataEngine::Vector3 max;

};

KamataEngine::Matrix4x4 Multiply(KamataEngine::Matrix4x4 matrix1, KamataEngine::Matrix4x4 matrix2);
void MakeAffineMatrix(KamataEngine::WorldTransform* worldTransform);
float EaseInSine(float self, float target, float t, float maxT);
float EaseOutSine(float self, float target, float t, float maxT);
KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& a, const KamataEngine::Vector3& b, float t);
KamataEngine::Vector3 Vector3Add(KamataEngine::Vector3 a, KamataEngine::Vector3 b);
KamataEngine::Vector3 Vector3Sub(KamataEngine::Vector3 a, KamataEngine::Vector3 b);
float RadToDeg(float radian);
int IsCollisionAABB2D(const AABB& a, const AABB& b);
KamataEngine::Matrix4x4 MakeRotateZMatrix(float radian);
void InitializeRandom();
float RandomFloat(float min, float max);
int RandomInt(int min, int max);




KamataEngine::Vector3 TransformM4x4ToV3(KamataEngine::Vector3 vector, KamataEngine::Matrix4x4 matrix);