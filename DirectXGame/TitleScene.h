#pragma once
#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "TextureManager.h"
class TitleScene {
private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	ViewProjection viewProjection_;
	WorldTransform worldTransform_title;
	WorldTransform worldTransform_player;
	Model* modelTitle_ = nullptr;
	Model* modelPlayer_ = nullptr;

	float titleSpeed_ = 0.03f;
	float titleMoveTime_ = 1.5f;
	float currentTime_ = 0;
	uint32_t textureHandle_ = 0;
	Sprite* sprite_ = nullptr;

public:
	~TitleScene();
	void Initalize();
	void Update();
	void Draw();
};