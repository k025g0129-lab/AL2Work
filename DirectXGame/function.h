#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include <vector>

KamataEngine::Matrix4x4 Multiply(KamataEngine::Matrix4x4 matrix1, KamataEngine::Matrix4x4 matrix2);
void MakeAffineMatrix(KamataEngine::WorldTransform* worldTransform);
float EaseInSine(float self, float target, float t, float maxT);