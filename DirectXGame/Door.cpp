#include "Door.h"

void Door::Initialize(ViewProjection* viewProjection, const Vector3& position)
{
    viewProjection_ = viewProjection;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
    model_ = Model::CreateFromOBJ("Door", true); 
}
void Door::Update()
{
    worldTransform_.UpdateMatrix();
}
void Door::Draw() {

    model_->Draw(worldTransform_, *viewProjection_);
}

