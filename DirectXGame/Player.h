#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include"ViewProjection.h"
#include <cassert>
class Player {
public:
	public: 
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model,ViewProjection* viewProjection);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	WorldTransform worldTransform_;
private:
	ViewProjection* viewProjection_ = nullptr;
	
	Model* model_ = nullptr;
};