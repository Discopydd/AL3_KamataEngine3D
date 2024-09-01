#pragma once
#include"ViewProjection.h"
#include"Player.h"
#include "WorldTransform.h"
#include <algorithm>

struct Rect {
	float left = 0;
	float right = 1;
	float top = 1;
	float bottom = 0;
};

class CameraController
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
    /// 追従対象を設定する
    /// </summary>
    void SetTarget(Player* target) { target_ = target; }

	void Reset();

	void SetMovableArea(Rect area) { movableArea_ = area; }

	const ViewProjection& GetViewProjection() { return viewProjection_; };
private:
	ViewProjection viewProjection_;
	Player* target_ = nullptr;

	// 追従対象とカメラの座標の差 (オフセット)
    Vector3 targetOffset_ = { 0, 0, -50.0f };
	Vector3 targetPos_;                    // 目標の位置
	const float kInterpolationRate = 0.2f;// 座標補間
	static inline const float kVelocityBias = 6.0f;        // 速度掛け率
	Rect movableArea_;
	Rect targetArea_ = {0, 100, 100, 0};
	
};

