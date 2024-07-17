#include "Player.h"
#include <imgui.h>




AABB Player::GetAABB()
{
    Vector3 worldPos = GetWorldPosition();
	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Player::OnCollision(const bool flag)
{
   isEnemyHit = flag;
    if (flag) {
        isDead = true;
        velocity_ = {0, 0, 0};
    }
}

Player::~Player() {}

void Player::Initialize(ViewProjection*viewProjection, const Vector3& position)
{
	worldTransform_.Initialize();
	viewProjection_ = viewProjection;
	worldTransform_.translation_ = position;
	model_ = Model::CreateFromOBJ("Player", true);
	
}

void Player::Update() {
    	if (isDead) {
		return;
	}
    #pragma region 1.移動入力
    // 右キーまたは左キーが押されたかどうかをチェックする
    if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
        Vector3 acceleration{};
        
        // 右キーが押された場合の処理
        if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
            // 現在の方向が右でない場合、方向と回転パラメータを更新する
            if (lrDirection_ != LRDirection::kRight) {
                lrDirection_ = LRDirection::kRight;
                turnStartRotationY_ = worldTransform_.rotation_.y;
                turnUseRotationY_ = 0 - worldTransform_.rotation_.y;
                turnNowFram_ = 1;
            }
            // 現在の速度が左向きの場合、速度を減衰させる
            if (velocity_.x < 0) {
                velocity_.x *= (1 - kAttenuation);
            }
            // 加速度を増加させる
            acceleration.x += kAcceleration;
        } else if (Input::GetInstance()->PushKey(DIK_LEFT)) { // 左キーが押された場合の処理
            // 現在の方向が左でない場合、方向と回転パラメータを更新する
            if (lrDirection_ != LRDirection::kLeft) {
                lrDirection_ = LRDirection::kLeft;
                turnStartRotationY_ = worldTransform_.rotation_.y;
                turnUseRotationY_ = acosf(-1) - worldTransform_.rotation_.y;
                turnNowFram_ = 1;
            }
            // 現在の速度が右向きの場合、速度を減衰させる
            if (velocity_.x > 0) {
                velocity_.x *= (1 - kAttenuation);
            }
            // 加速度を減少させる
            acceleration.x -= kAcceleration;
        }
        
        // 速度を更新する
        velocity_ += acceleration;
        // 速度範囲を制限する
        velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
    } else {
        // 左右キーが押されていない場合、速度を徐々に減衰させる
        velocity_.x *= (1 - kAttenuation);
    }
      // ジャンプ中の場合、重力加速度を適用する
   
        velocity_ += Vector3(0, -kGravityAcceleration_, 0);
        velocity_.y = max(velocity_.y, -kLimitFallSpeed_);
  
    // ジャンプの処理
    if (!isJump_) {
        // ジャンプしていない場合、上キーが押されたかどうかをチェックする
        if (Input::GetInstance()->PushKey(DIK_UP)) {
            isJump_ = true;
            onGround_ = false;
            // 上向きの速度を増加させる
            velocity_ += Vector3(0, kJumpAcceleration_, 0);
        }
    } else {
    }
    #pragma endregion

    // 2.移動量を加味して衝突判定する
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;
	MapCollision(collisionMapInfo);

    // 3.判定結果を反映して移動させる
    worldTransform_.translation_ += collisionMapInfo.move;

    // 4.天井に接触している場合の処理
	if (collisionMapInfo.ceiling) {
		velocity_.y = 0;
	}
    // 5.壁に接触している場合の処理
	if (collisionMapInfo.hitWall) {
		velocity_.x *= (1 - kAccelerationWall);
	}
    // 6.接地状態の切り替え
	MapCollision_isGroundChange(collisionMapInfo);
    // 7.回転アニメーションの処理
    if (turnNowFram_ >= 1 && turnNowFram_ < turnEndFrame_) {
        turnNowFram_++;
        float easing = powf(float(turnNowFram_) / float(turnEndFrame_), 3);
        worldTransform_.rotation_.y = turnUseRotationY_ * easing + turnStartRotationY_;
    }

    // 8.ワールド変換行列を更新する
    worldTransform_.UpdateMatrix();

    ImGui::Begin("A");
	ImGui::SliderFloat3("velocity", &velocity_.x, 0.0f, 1.0f);
	ImGui::Checkbox("onGround_", &onGround_);
	ImGui::End();
}


void Player::Draw()
{
	if (isDead)
		return;
	model_->Draw(worldTransform_, *viewProjection_);
}

void Player::MapCollision(CollisionMapInfo& info) {   MapCollision_Up(info);
    MapCollision_Down(info);
    MapCollision_Left(info);
    MapCollision_Right(info);
 }
void Player::MapCollision_Up(CollisionMapInfo& info)
{
#pragma region マップ衝突判定上方向
    // 移動後の４つ角の座標
	std ::array<Vector3, static_cast<int>(kNumCorner)> positionsNew{};
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}
    // もし上に移動してないなら、判断の必要がない
	if (info.move.y <= 0)
		return;
    bool hit = false;
	MapChipField::IndexSet indexSet{};
	MapChipType mapChipType{};
	// 左上
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右上
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
    if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		float moveY = rect.bottom - positionsNew[kLeftTop].y - kHeight / 2 + kBlank;
		info.move.y = max(0.0f, moveY);
		// 天井に当たったことを記録する
		info.ceiling = true;
	}
    #pragma endregion
}

void Player::MapCollision_Down(CollisionMapInfo& info)
{
    std ::array<Vector3, static_cast<int>(kNumCorner)> positionsNew{};
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

      if (info.move.y >= 0) return;

    bool hit = false;
    MapChipField::IndexSet indexSet{};
    MapChipType mapChipType{};
    
    // 左下
    indexSet = mapChipField_->GetMapChipIndexByPosition(CornerPosition(worldTransform_.translation_ + info.move, kLeftBottom));
    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
    if (mapChipType == MapChipType::kBlock) hit = true;

    // 右下
    indexSet = mapChipField_->GetMapChipIndexByPosition(CornerPosition(worldTransform_.translation_ + info.move, kRightBottom));
    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
    if (mapChipType == MapChipType::kBlock) hit = true;

    if (hit) {
        
        indexSet = mapChipField_->GetMapChipIndexByPosition(CornerPosition(worldTransform_.translation_ + info.move, kLeftBottom));
        MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
        float moveY = rect.top - CornerPosition(worldTransform_.translation_ + info.move, kLeftBottom).y + kHeight / 2 - kBlank;
        info.move.y = min(0.0f, moveY);
        info.landing = true;
    }
}

void Player::MapCollision_Left(CollisionMapInfo& info)
{
    std ::array<Vector3, static_cast<int>(kNumCorner)> positionsNew{};
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

        if (info.move.x >= 0) return;

    bool hit = false;
    MapChipField::IndexSet indexSet{};
    MapChipType mapChipType{};
    
    // 左上
    indexSet = mapChipField_->GetMapChipIndexByPosition(CornerPosition(worldTransform_.translation_ + info.move, kLeftTop));
    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
    if (mapChipType == MapChipType::kBlock) hit = true;

    // 左下
    indexSet = mapChipField_->GetMapChipIndexByPosition(CornerPosition(worldTransform_.translation_ + info.move, kLeftBottom));
    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
    if (mapChipType == MapChipType::kBlock) hit = true;

    if (hit) {
        // 排除左方向的穿透
        indexSet = mapChipField_->GetMapChipIndexByPosition(CornerPosition(worldTransform_.translation_ + info.move, kLeftBottom));
        MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
        float moveX = rect.right - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank);
        info.move.x = max(0.0f, moveX);
        info.hitWall = true;
    }
}

void Player::MapCollision_Right(CollisionMapInfo& info)
{
    std ::array<Vector3, static_cast<int>(kNumCorner)> positionsNew{};
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

      if (info.move.x <= 0) return;

    bool hit = false;
    MapChipField::IndexSet indexSet{};
    MapChipType mapChipType{};
    
    // 右上
    indexSet = mapChipField_->GetMapChipIndexByPosition(CornerPosition(worldTransform_.translation_ + info.move, kRightTop));
    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
    if (mapChipType == MapChipType::kBlock) hit = true;

    // 右下
    indexSet = mapChipField_->GetMapChipIndexByPosition(CornerPosition(worldTransform_.translation_ + info.move, kRightBottom));
    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
    if (mapChipType == MapChipType::kBlock) hit = true;

    if (hit) {
        // 排除右方向的穿透
        indexSet = mapChipField_->GetMapChipIndexByPosition(CornerPosition(worldTransform_.translation_ + info.move, kRightBottom));
        MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
        float moveX = rect.left - worldTransform_.translation_.x + (kWidth / 2.0f + kBlank);
        info.move.x = min(0.0f, moveX);
        info.hitWall = true;
    }
}

void Player::MapCollision_isGroundChange(const CollisionMapInfo& info)
{
    if (onGround_) {
        if (velocity_.y > 0) {
            onGround_ = false;
        }
    }else {
		if (info.landing) {
			onGround_ = true;
			isJump_ = false;
			velocity_.x *= (1 - kAttenuationLanding);
			velocity_.y = 0;
		}
	}
}

Vector3 Player::GetWorldPosition()
{
	//ワールド座標を入れる変数
	Vector3 worldPos;
	//ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;
	
	return worldPos;
}



Vector3 Player::CornerPosition(const Vector3& center, Corner corner)
{
    Vector3 offsetTable[kNumCorner] = {
   {+kWidth / 2.0f, -kHeight / 2.0f, 0.0f},  // kRightBottom
   {-kWidth / 2.0f, -kHeight / 2.0f, 0.0f},  // kLeftBottom
   {+kWidth / 2.0f, +kHeight / 2.0f, 0.0f},  // kRightTop
   {-kWidth / 2.0f, +kHeight / 2.0f, 0.0f}   // kLeftTop
    };
    return center + offsetTable[static_cast<uint32_t>(corner)];
}