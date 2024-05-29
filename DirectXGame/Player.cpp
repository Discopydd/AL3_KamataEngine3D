#include "Player.h"

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
	worldTransform_.UpdateMatrix();
}

void Player::Draw()
{
	model_->Draw(worldTransform_, *viewProjection_);
}