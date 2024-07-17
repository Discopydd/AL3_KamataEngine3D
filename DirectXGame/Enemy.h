#pragma once
#include "Model.h"
#include "Vector3.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "MyMath.h"
/// <summary>
/// 敵
/// </summary>
class Player;
class Enemy {
private:
	// 補講の速さ
	static inline const float kWalkSpeed = 0.03f;
	//最初の角度[度]
	static inline const float kWalkMotionAngleStart = 10.0f;
	//最後の角度[度]
	static inline const float kWalkMotionAngleEnd = -40.0f;
	//アニメーションの周期となる時間[秒]
	static inline const float kWalklMotionTime = 1.0f;
	//経過時間
	float walkTimer_ = 0.0f;

	static inline const float kWidth = 2.0f;
	static inline const float kHeight = 2.0f;

	bool isPlayerHit = false;

	Vector3 velocity_ = {};

	WorldTransform worldTransform_;

	Model* model_ = nullptr;

	ViewProjection* viewProjection_ = nullptr;

public:

	void Initialize(ViewProjection* viewProjection, const Vector3& position);

	void Update();

	void Draw();

	//AABBを取得
	AABB GetAABB();

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	//衝突応答
	void OnCollision(const bool flag);

};