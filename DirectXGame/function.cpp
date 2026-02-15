#include "function.h"

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
