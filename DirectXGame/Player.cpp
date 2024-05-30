#include "Player.h"

Player::~Player() { delete model_; }

void Player::Initialize(Model* model,ViewProjection*viewProjection, const Vector3& position)
{
	assert(model);
	worldTransform_.Initialize();
	viewProjection_ = viewProjection;
	worldTransform_.translation_ = position;
	model_ = Model::CreateFromOBJ("Player", true);
	
}

void Player::Update()
{
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
			acceleration.x -= kAttenuation;
		}

		velocity_ += acceleration;
		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
	} else {
		velocity_.x *= (1 - kAttenuation);
	}
	//
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
	if (velocity_.y < 0) {
		if (worldTransform_.translation_.y <= 2) {
			isJump_ = false;
			onGround_ = true;
			worldTransform_.translation_.y = 2;
			velocity_.y = 0;
		}
	}

	worldTransform_.translation_ += velocity_;

	if (turnNowFram_ >= 1 && turnNowFram_ < turnEndFrame_) {
		turnNowFram_++;
		float easing = powf(float(turnNowFram_) / float(turnEndFrame_), 3);
		worldTransform_.rotation_.y = turnUseRotationY_ * easing + turnStartRotationY_;
	}

	worldTransform_.UpdateMatrix();
}

void Player::Draw()
{
	model_->Draw(worldTransform_, *viewProjection_);
}