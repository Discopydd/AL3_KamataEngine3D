#include "Enemy.h"
#include "Mymath.h"
#include <numbers>

void Enemy::Initialize(ViewProjection* viewProjection, const Vector3& position) { 
	viewProjection_ = viewProjection;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / -2.0f;
	velocity_ = {-kWalkSpeed, 0, 0};
	walkTimer_ = 0.0f;
	model_ = Model::CreateFromOBJ("enemy", true);
}

void Enemy::Update() {
	// 移動
	worldTransform_.translation_.x += velocity_.x;
	// タイマーを加算
	walkTimer_ += 1.0f / 60.0f;
    //回転アニメーション
	float param = std::sin(2.0f * std::numbers::pi_v<float> * walkTimer_ / kWalklMotionTime);
	float radian = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.z = radian * (std::numbers::pi_v<float> / 180.0f);
	// 行列計算                 
	worldTransform_.UpdateMatrix();
}

void Enemy::Draw() { model_->Draw(worldTransform_, *viewProjection_); }