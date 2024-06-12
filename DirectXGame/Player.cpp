#include "Player.h"

Player::~Player() {}

void Player::Initialize(ViewProjection*viewProjection, const Vector3& position)
{
	worldTransform_.Initialize();
	viewProjection_ = viewProjection;
	worldTransform_.translation_ = position;
	model_ = Model::CreateFromOBJ("Player", true);
	
}

void Player::Update() {
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
        // ジャンプ中の場合、重力加速度を適用する
        velocity_ += Vector3(0, -kGravityAcceleration_, 0);
        // 落下速度を制限する
        velocity_.y = max(velocity_.y, -kLimitFallSpeed_);
    }

    // 着地の処理
    if (velocity_.y < 0) {
        // プレイヤーが地面に接触したかどうかをチェックする
        if (worldTransform_.translation_.y <= 2) {
            isJump_ = false;
            onGround_ = true;
            worldTransform_.translation_.y = 2;
            velocity_.y = 0;
        }
    }

    // 位置を更新する
    worldTransform_.translation_ += velocity_;

    // 回転アニメーションの処理
    if (turnNowFram_ >= 1 && turnNowFram_ < turnEndFrame_) {
        turnNowFram_++;
        float easing = powf(float(turnNowFram_) / float(turnEndFrame_), 3);
        worldTransform_.rotation_.y = turnUseRotationY_ * easing + turnStartRotationY_;
    }

    // ワールド変換行列を更新する
    worldTransform_.UpdateMatrix();
}


void Player::Draw()
{
	model_->Draw(worldTransform_, *viewProjection_);
}