#include "Player.h"

Player::~Player() {}

void Player::Initialize(ViewProjection* viewProjection, const Vector3& position) {
    worldTransform_.Initialize();
    viewProjection_ = viewProjection;
    worldTransform_.translation_ = position;
    model_ = Model::CreateFromOBJ("Player", true);
}

void Player::Update() {
    #pragma region 1.移動入力
    if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
        Vector3 acceleration{};

        if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
            if (lrDirection_ != LRDirection::kRight) {
                lrDirection_ = LRDirection::kRight;
                turnStartRotationY_ = worldTransform_.rotation_.y;
                turnUseRotationY_ = 0 - worldTransform_.rotation_.y;
                turnNowFram_ = 1;
            }
            if (velocity_.x < 0) {
                velocity_.x *= (1 - kAttenuation);
            }
            acceleration.x += kAcceleration;
        } else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
            if (lrDirection_ != LRDirection::kLeft) {
                lrDirection_ = LRDirection::kLeft;
                turnStartRotationY_ = worldTransform_.rotation_.y;
                turnUseRotationY_ = acosf(-1) - worldTransform_.rotation_.y;
                turnNowFram_ = 1;
            }
            if (velocity_.x > 0) {
                velocity_.x *= (1 - kAttenuation);
            }
            acceleration.x -= kAcceleration;
        }

        velocity_ += acceleration;
        velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
    } else {
        velocity_.x *= (1 - kAttenuation);
    }

    if (!isJump_) {
        if (Input::GetInstance()->PushKey(DIK_UP)) {
            isJump_ = true;
            onGround_ = false;
            velocity_ += Vector3(0, kJumpAcceleration_, 0);
        }
    } else {
        velocity_ += Vector3(0, -kGravityAcceleration_, 0);
        velocity_.y = max(velocity_.y, -kLimitFallSpeed_);
    }

    #pragma endregion

    #pragma region 2.移動量を加味して衝突判定する
    CollisionMapInfo collisionMapInfo;
    collisionMapInfo.move = velocity_;
    MapCollision(collisionMapInfo);

    #pragma region 3.判定結果を反映して移動させる
    worldTransform_.translation_ += collisionMapInfo.move;

    #pragma region 4.天井に接触している場合の処理
    if (collisionMapInfo.ceiling) {
        velocity_.y = 0;
    }

    #pragma region 5.壁に接触している場合の処理
    if (collisionMapInfo.hitWall) {
        velocity_.x *= (1 - kAccelerationWall);
    }

    #pragma region 6.接地状態の切り替え
    MapCollision_isGroundChange(collisionMapInfo);

    #pragma region 7.回転アニメーションの処理
    if (turnNowFram_ >= 1 && turnNowFram_ < turnEndFrame_) {
        turnNowFram_++;
        float easing = powf(float(turnNowFram_) / float(turnEndFrame_), 3);
        worldTransform_.rotation_.y = turnUseRotationY_ * easing + turnStartRotationY_;
    }

    #pragma region 8.ワールド変換行列を更新する
    worldTransform_.UpdateMatrix();

    #pragma region 9.跳跃后的位置校正
    CorrectPositionAfterJump();
}

void Player::Draw() {
    model_->Draw(worldTransform_, *viewProjection_);
}

void Player::MapCollision(CollisionMapInfo& info) {
    MapCollision_Up(info);
    MapCollision_Down(info);
    MapCollision_Left(info);
    MapCollision_Right(info);
}

void Player::MapCollision_Up(CollisionMapInfo& info) {
    if (info.move.y <= 0)
        return;

    std::array<Vector3, static_cast<int>(kNumCorner)> positionsNew{};
    for (uint32_t i = 0; i < positionsNew.size(); ++i) {
        positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
    }

    bool hit = false;
    MapChipField::IndexSet indexSet{};
    MapChipType mapChipType{};
    Vector3 offset(0, 1, 0);

    positionsNew[kLeftTop] += offset;
    indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);
    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
    if (mapChipType == MapChipType::kBlock) {
        hit = true;
    }

    positionsNew[kRightTop] += offset;
    indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
    if (mapChipType == MapChipType::kBlock) {
        hit = true;
    }

    if (hit) {
        indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);
        MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
        float moveY = rect.bottom - positionsNew[kLeftTop].y - kHeight / 2 + kBlank;
        info.move.y = max(0.0f, moveY);
        info.ceiling = true;
    }
}

void Player::MapCollision_Down(CollisionMapInfo& info) {
    if (info.move.y >= 0)
        return;

    std::array<Vector3, static_cast<int>(kNumCorner)> positionsNew{};
    for (uint32_t i = 0; i < positionsNew.size(); ++i) {
        positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
    }

    bool hit = false;
    MapChipField::IndexSet indexSet{};
    MapChipType mapChipType{};
    Vector3 offset(0, 1, 0);

    positionsNew[kLeftBottom] += offset;
    indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom]);
    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
    if (mapChipType == MapChipType::kBlock) {
        hit = true;
    }

    positionsNew[kRightBottom] += offset;
    indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightBottom]);
    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
    if (mapChipType == MapChipType::kBlock) {
        hit = true;
    }

    if (hit) {
        indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom]);
        MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
        float moveY = rect.top - worldTransform_.translation_.y + kHeight / 2 + kBlank;
        info.move.y = min(0.0f, moveY);
        info.landing = true;
    }
}

void Player::MapCollision_Left(CollisionMapInfo& info) {
    if (info.move.x >= 0)
        return;

    std::array<Vector3, static_cast<int>(kNumCorner)> positionsNew{};
    for (uint32_t i = 0; i < positionsNew.size(); ++i) {
        positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
    }

    bool hit = false;
    MapChipField::IndexSet indexSet{};
    MapChipType mapChipType{};
    Vector3 offset(0, 1, 0);

    positionsNew[kLeftTop] += offset;
    indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);
    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
    if (mapChipType == MapChipType::kBlock) {
        hit = true;
    }

    positionsNew[kLeftBottom] += offset;
    indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom]);
    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
    if (mapChipType == MapChipType::kBlock) {
        hit = true;
    }

    if (hit) {
        indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);
        MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
        float moveX = rect.right - worldTransform_.translation_.x + kWidth / 2 + kBlank;
        info.move.x = min(0.0f, moveX);
        info.hitWall = true;
    }
}

void Player::MapCollision_Right(CollisionMapInfo& info) {
    if (info.move.x <= 0)
        return;

    std::array<Vector3, static_cast<int>(kNumCorner)> positionsNew{};
    for (uint32_t i = 0; i < positionsNew.size(); ++i) {
        positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
    }

    bool hit = false;
    MapChipField::IndexSet indexSet{};
    MapChipType mapChipType{};
    Vector3 offset(0, 1, 0);

    positionsNew[kRightTop] += offset;
    indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
    if (mapChipType == MapChipType::kBlock) {
        hit = true;
    }

    positionsNew[kRightBottom] += offset;
    indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightBottom]);
    mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
    if (mapChipType == MapChipType::kBlock) {
        hit = true;
    }

    if (hit) {
        indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
        MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
        float moveX = rect.left - worldTransform_.translation_.x - kWidth / 2 + kBlank;
        info.move.x = max(0.0f, moveX);
        info.hitWall = true;
    }
}

void Player::MapCollision_isGroundChange(const CollisionMapInfo& info) {
    if (onGround_) {
        if (velocity_.y > 0) {
            onGround_ = false;
        } else {
            std::array<Vector3, static_cast<int>(kNumCorner)> positionsNew{};
            for (uint32_t i = 0; i < positionsNew.size(); ++i) {
                positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
            }

            bool hit = false;
            MapChipField::IndexSet indexSet{};
            MapChipType mapChipType{};
            Vector3 offset(0, 1, 0);

            positionsNew[kLeftBottom] += offset;
            indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom]);
            mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
            if (mapChipType == MapChipType::kBlock) {
                hit = true;
            }

            positionsNew[kRightBottom] += offset;
            indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightBottom]);
            mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
            if (mapChipType == MapChipType::kBlock) {
                hit = true;
            }

            if (!hit) {
                onGround_ = false;
            }
        }
    } else {
        if (info.landing) {
            onGround_ = true;
            isJump_ = false;
            velocity_.x *= (1 - kAttenuationLanding);
            velocity_.y = 0;
        }
    }
}

void Player::CorrectPositionAfterJump() {
    if (!onGround_ && isJump_) {
        CollisionMapInfo collisionMapInfo;
        collisionMapInfo.move = Vector3(0, -0.1f, 0);  // 向下校正位置
        MapCollision(collisionMapInfo);
        if (collisionMapInfo.landing) {
            worldTransform_.translation_.y += collisionMapInfo.move.y;
        }
    }
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
    Vector3 offsetTable[kNumCorner] = {
        {+kWidth / 2.0f, -kHeight / 2.0f, 0.0f},  // kRightBottom
        {-kWidth / 2.0f, -kHeight / 2.0f, 0.0f},  // kLeftBottom
        {+kWidth / 2.0f, +kHeight / 2.0f, 0.0f},  // kRightTop
        {-kWidth / 2.0f, +kHeight / 2.0f, 0.0f}   // kLeftTop
    };
    return center + offsetTable[static_cast<uint32_t>(corner)];
}
