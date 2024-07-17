#pragma once
#include "TextureManager.h"
#include <cassert>
#include "Audio.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <vector>
#include"Player.h"
#include"MapChipField.h"
#include "CameraController.h"
#include "Skydome.h"
#include "Enemy.h"
#include <imgui.h>
#include "DeathParticles.h"
/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

    //全ての当たり判定を行う
	void CheckAllCollisions();

    //Scene
	bool isSceneOver = false;

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// 自分の変数
	ViewProjection viewProjection_;
	Model* model_ = nullptr;
	//DebugCamera
	bool isDebugCameraActrive_ = false;
	DebugCamera* debugCamera_ = nullptr;
	// Skydome
	Skydome* skydomeObj_ = nullptr;
	// Player
	Player* player_ = nullptr;
	//敵
	std::list<Enemy*>enemies_;
	int32_t enemyCount = 1;
	//Map
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	MapChipField* mapChipField_;
	void GenerateBlocks();
	// CameraController
	CameraController* cameraController_ = nullptr; 

	// Particles
	DeathParticles* deathParticles_ = nullptr;
	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
