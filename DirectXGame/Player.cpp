#include "Player.h"

void Player::Initialize(Model* model,ViewProjection*viewProjection)
{
	assert(model);
	worldTransform_.Initialize();
	viewProjection_ = viewProjection;
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