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
#include "MyMath.h"
class MapChipField;
class Enemy;
class Player {
private:
	ViewProjection* viewProjection_ = nullptr;
	WorldTransform worldTransform_;
	Model* model_ = nullptr;

	Vector3 velocity_{};
	const float kAcceleration = 0.1f;
	const float kAttenuation = 0.2f;   // 速度減衰
	const float kLimitRunSpeed = 0.3f; // 最大速度

	enum class LRDirection {
		kRight,//右
		kLeft,//左
	};
	LRDirection lrDirection_ = LRDirection::kRight;
	float turnStartRotationY_ = 0;
	float turnUseRotationY_ = 0; 
	int turnNowFram_ = 0;
	const int turnEndFrame_ = 10;

	bool onGround_ = true;
	bool isJump_ = false;
	bool isEnemyHit = false;
	bool isDead = false;

	const float kGravityAcceleration_ = 0.05f;
	const float kLimitFallSpeed_ = 0.4f;
	const float kJumpAcceleration_ = 1.0f;

	const float kAttenuationLanding = 0.05f;
	const float kAccelerationWall = 0.06f;
	//mapの判定変数
	MapChipField* mapChipField_ = nullptr;
	//キャラクターの当たり判定サイズ
	static inline const float kWidth = 2.0f;
	static inline const float kHeight = 2.0f;

	static inline const float kBlank = 0.01f;
	// マップと当たり情報
	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
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

	void MapCollision_Down(CollisionMapInfo& info);

	void MapCollision_Left(CollisionMapInfo& info);

	void MapCollision_Right(CollisionMapInfo& info);

	void MapCollision_isGroundChange(const CollisionMapInfo& info);




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

	//AABBを取得
	AABB GetAABB();

	//衝突応答
	void OnCollision(const bool flag);

	//ワールド座標を取得
	Vector3 GetWorldPosition();

	void SetDead(bool dead) { isDead = dead; }
	bool GetDead() const { return isDead; }
};