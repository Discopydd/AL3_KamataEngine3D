#include"MyMath.h"

Matrix4x4 Multiply(const Matrix4x4& matrix1, const Matrix4x4& matrix2) {
    Matrix4x4 result;
    result.m[0][0] = matrix1.m[0][0] * matrix2.m[0][0] + matrix1.m[0][1] * matrix2.m[1][0] +
	                 matrix1.m[0][2] * matrix2.m[2][0]+ matrix1.m[0][3] * matrix2.m[3][0];
	result.m[0][1] = matrix1.m[0][0] * matrix2.m[0][1] + matrix1.m[0][1] * matrix2.m[1][1] +
	                 matrix1.m[0][2] * matrix2.m[2][1]+ matrix1.m[0][3] * matrix2.m[3][1];
	result.m[0][2] = matrix1.m[0][0] * matrix2.m[0][2] + matrix1.m[0][1] * matrix2.m[1][2] +
	                 matrix1.m[0][2] * matrix2.m[2][2]+ matrix1.m[0][3] * matrix2.m[3][2];
    result.m[0][3] = matrix1.m[0][0] * matrix2.m[0][3] + matrix1.m[0][1] * matrix2.m[1][3] +
	                 matrix1.m[0][2] * matrix2.m[2][3]+ matrix1.m[0][3] * matrix2.m[3][3];

	result.m[1][0] = matrix1.m[1][0] * matrix2.m[0][0] + matrix1.m[1][1] * matrix2.m[1][0] +
	                 matrix1.m[1][2] * matrix2.m[2][0] + matrix1.m[1][3] * matrix2.m[3][0];
	result.m[1][1] = matrix1.m[1][0] * matrix2.m[0][1] + matrix1.m[1][1] * matrix2.m[1][1] +
	                 matrix1.m[1][2] * matrix2.m[2][1] + matrix1.m[1][3] * matrix2.m[3][1];
	result.m[1][2] = matrix1.m[1][0] * matrix2.m[0][2] + matrix1.m[1][1] * matrix2.m[1][2] +
	                 matrix1.m[1][2] * matrix2.m[2][2] + matrix1.m[1][3] * matrix2.m[3][2];
    result.m[1][3] = matrix1.m[1][0] * matrix2.m[0][3] + matrix1.m[1][1] * matrix2.m[1][3] +
	                 matrix1.m[1][2] * matrix2.m[2][3] + matrix1.m[1][3] * matrix2.m[3][3];

	result.m[2][0] = matrix1.m[2][0] * matrix2.m[0][0] + matrix1.m[2][1] * matrix2.m[1][0] +
	                 matrix1.m[2][2] * matrix2.m[2][0]+matrix1.m[2][3] * matrix2.m[3][0];
	result.m[2][1] = matrix1.m[2][0] * matrix2.m[0][1] + matrix1.m[2][1] * matrix2.m[1][1] +
	                 matrix1.m[2][2] * matrix2.m[2][1]+matrix1.m[2][3] * matrix2.m[3][1];
	result.m[2][2] = matrix1.m[2][0] * matrix2.m[0][2] + matrix1.m[2][1] * matrix2.m[1][2] +
	                 matrix1.m[2][2] * matrix2.m[2][2]+matrix1.m[2][3] * matrix2.m[3][2];
    result.m[2][3] = matrix1.m[2][0] * matrix2.m[0][3] + matrix1.m[2][1] * matrix2.m[1][3] +
	                 matrix1.m[2][2] * matrix2.m[2][3]+matrix1.m[2][3] * matrix2.m[3][3];

    result.m[3][0] = matrix1.m[3][0] * matrix2.m[0][0] + matrix1.m[3][1] * matrix2.m[1][0] +
	                 matrix1.m[3][2] * matrix2.m[2][0]+matrix1.m[3][3] * matrix2.m[3][0];
	result.m[3][1] = matrix1.m[3][0] * matrix2.m[0][1] + matrix1.m[3][1] * matrix2.m[1][1] +
	                 matrix1.m[3][2] * matrix2.m[2][1]+matrix1.m[3][3] * matrix2.m[3][1];
	result.m[3][2] = matrix1.m[3][0] * matrix2.m[0][2] + matrix1.m[3][1] * matrix2.m[1][2] +
	                 matrix1.m[3][2] * matrix2.m[2][2]+matrix1.m[3][3] * matrix2.m[3][2];
    result.m[3][3] = matrix1.m[3][0] * matrix2.m[0][3] + matrix1.m[3][1] * matrix2.m[1][3] +
	                 matrix1.m[3][2] * matrix2.m[2][3]+matrix1.m[3][3] * matrix2.m[3][3];
    return result;
}
Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotation, Vector3 translation) {
	// Scale
	Matrix4x4 mScale = {0};
	mScale.m[0][0] = scale.x;
	mScale.m[1][1] = scale.y;
	mScale.m[2][2] = scale.z;
	mScale.m[3][3] = 1;
	// Rotation
	Matrix4x4 mRotationZ = {0};
	mRotationZ.m[0][0] = cosf(rotation.z);
	mRotationZ.m[0][1] = sinf(rotation.z);
	mRotationZ.m[1][0] = -sinf(rotation.z);
	mRotationZ.m[1][1] = cosf(rotation.z);
	mRotationZ.m[2][2] = mRotationZ.m[3][3] = 1;
	Matrix4x4 mRotationX = {0};
	mRotationX.m[1][1] = cosf(rotation.x);
	mRotationX.m[1][2] = sinf(rotation.x);
	mRotationX.m[2][1] = -sinf(rotation.x);
	mRotationX.m[2][2] = cosf(rotation.x);
	mRotationX.m[0][0] = mRotationX.m[3][3] = 1;
	Matrix4x4 mRotationY = {0};
	mRotationY.m[0][0] = cosf(rotation.y);
	mRotationY.m[2][0] = sinf(rotation.y);
	mRotationY.m[0][2] = -sinf(rotation.y);
	mRotationY.m[2][2] = cosf(rotation.y);
	mRotationY.m[1][1] = mRotationY.m[3][3] = 1;
	Matrix4x4 mRotation = Multiply(mRotationX, Multiply(mRotationY, mRotationZ));
	// Translation
	Matrix4x4 mTranslation = {0};
	mTranslation.m[0][0] = mTranslation.m[1][1] = mTranslation.m[2][2] = mTranslation.m[3][3] = 1;
	mTranslation.m[3][0] = translation.x;
	mTranslation.m[3][1] = translation.y;
	mTranslation.m[3][2] = translation.z;

	return Multiply(mScale, Multiply(mRotation, mTranslation));
}