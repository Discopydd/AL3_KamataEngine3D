#pragma once
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
/// <summary>
/// 天球
/// </summary>
class Skydome {
private:
	WorldTransform worldTransform_;
	ViewProjection* viewProjection_ = nullptr;
	Model* model_ = nullptr;

public:
	~Skydome();
	void Initialize(ViewProjection* view);
	void Update();
	void Draw();
};
