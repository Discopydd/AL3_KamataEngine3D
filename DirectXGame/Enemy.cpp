#include "Enemy.h"
#include "Mymath.h"
#include <numbers>

void Enemy::Initialize(ViewProjection* viewProjection, const Vector3& position) { 
	viewProjection_ = viewProjection;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y= std::numbers::pi_v<float> / -2.0f;
	SetRandomVelocity(-0.2f, -0.05f);
	walkTimer_ = 0.0f;
	model_ = Model::CreateFromOBJ("enemy", true);

	 minX_ = position.x - 50.0f; 
    maxX_ = position.x + 50.0f;
}

void Enemy::Update() {
	// 移動
	worldTransform_.translation_.x += velocity_.x;
	
	// タイマーを加算
	walkTimer_ += 1.0f / 60.0f;
    //回転アニメーション
	float param = std::sin(2.0f * std::numbers::pi_v<float> * walkTimer_ / kWalklMotionTime);
	float radian = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.x = radian * (std::numbers::pi_v<float> / 180.0f);
	if (worldTransform_.translation_.x < minX_ || worldTransform_.translation_.x > maxX_) {
        velocity_.x = -velocity_.x; 
		  if (velocity_.x > 0) {
          worldTransform_.rotation_.y= std::numbers::pi_v<float> / 2.0f;
        } else {
           worldTransform_.rotation_.y= std::numbers::pi_v<float> / -2.0f;
        }
    }
	// 行列計算                 
	worldTransform_.UpdateMatrix();
}

void Enemy::Draw() { model_->Draw(worldTransform_, *viewProjection_); }

AABB Enemy::GetAABB()
{ 
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

Vector3 Enemy::GetWorldPosition()
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

void Enemy::OnCollision(const bool flag)
{
	isPlayerHit = flag;
}
void Enemy::SetRandomVelocity(float minSpeed, float maxSpeed) {
    std::random_device rd; 
    std::mt19937 gen(rd()); 
    std::uniform_real_distribution<float> dis(minSpeed, maxSpeed); 

    float randomSpeed = dis(gen);
    velocity_ = {randomSpeed, 0, 0}; 
}