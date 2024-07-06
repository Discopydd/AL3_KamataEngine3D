#pragma once
#include"Matrix4x4.h"
#include"Vector3.h"
#include <assert.h>
#include<cmath>
Matrix4x4 Multiply(const Matrix4x4& matrix1, const Matrix4x4& matrix2);
Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotation, Vector3 translation);
Matrix4x4 MakeRotateZMatrix(float radian);

Vector3 Transform(const Vector3 vector, Matrix4x4 matrix);
struct AABB {
	Vector3 min;
	Vector3 max;
};
bool IsCollision(const AABB& aabb1, const AABB& aabb2);

