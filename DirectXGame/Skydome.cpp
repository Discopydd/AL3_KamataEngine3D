#include "Skydome.h"

Skydome::~Skydome() { delete model_; }

void Skydome::Initialize(ViewProjection* view) {
	worldTransform_.Initialize();
	viewProjection_ = view;
	model_ = Model::CreateFromOBJ("Skydome", true);

	worldTransform_.scale_ = { 250, 250, 250 };
}

void Skydome::Update() { worldTransform_.UpdateMatrix(); }

void Skydome::Draw() { model_->Draw(worldTransform_, *viewProjection_); }
