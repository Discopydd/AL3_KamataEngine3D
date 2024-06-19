#pragma once
#include "Model.h"
#include "Input.h"
#include "DebugText.h"
#include "WorldTransform.h"
#include"ViewProjection.h"
#include"MapChipField.h"
#include <cassert>
#include <algorithm>
#include <cmath>
class MapChipField;
class Player {
private:
	ViewProjection* viewProjection_ = nullptr;
	WorldTransform worldTransform_;
	Model* model_ = nullptr;

	Vector3 velocity_{};
	const float kAcceleration = 0.1f;
	const float kAttenuation = 0.2f;   // 速度減衰
	const float kLimitRunSpeed = 0.4f; // 最大速度

	enum class LRDirection {
		kRight,//右
		kLeft,//左
	};
	LRDirection lrDirection_ = LRDirection::kRight;
	float turnStartRotationY_ = 0;
	float turnUseRotationY_ = 0; 
	int turnNowFram_ = 0;
	const int turnEndFrame_ = 10;

	bool isJump_ = false;
	bool onGround_ = true;
	const float kGravityAcceleration_ = 0.05f;
	const float kLimitFallSpeed_ = 0.4f;
	const float kJumpAcceleration_ = 1.0f;
	//mapの判定変数
	MapChipField* mapChipField_ = nullptr;
	//キャラクターの当たり判定サイズ
	static inline const float kWidth = 2.0f;
	static inline const float kHeight = 2.0f;

	static inline const float kBlank = 0.1f;
	// マップと当たり情報
	struct CollisionMapInfo {
		bool ceiling = false;
		bool isGround = false;
		bool isWall = false;
		Vector3 move{};
	};
	//角
	enum Corner {
		kRightBottom,    //右下
		kLeftBottom,     //左下
		kRightTop,       //右上
		kLeftTop,        //左上

		kNumCorner      //要素数
	};

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	void MapCollision(CollisionMapInfo& info);

	void MapCollision_Up(CollisionMapInfo& info);
	public: 
	~Player();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ViewProjection* viewProjection, const Vector3& position);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	
	const WorldTransform& GetWorldTransform() { return worldTransform_; };

	const Vector3& GetVelocity() const { return velocity_; };

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; };

	
};