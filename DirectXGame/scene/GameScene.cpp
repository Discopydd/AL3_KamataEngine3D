#include "GameScene.h"
void GameScene::GenerateBlocks() {
	// ブロックを初期化
	const uint32_t kNumBlockHorizontal = MapChipField::kNumBlockHorizontal;
	const uint32_t kNumBlockVertical = MapChipField::kNumBlockVirtical;
	worldTransformBlocks_.resize(kNumBlockVertical);
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}
	// ブロック生成
for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			switch (mapChipField_->GetMapChipTypeByIndex(j, i)) {
			case MapChipType::kBlock:
				worldTransformBlocks_[i][j] = new WorldTransform();
				worldTransformBlocks_[i][j]->Initialize();
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
				break;
			}
		}
	}
}
void GameScene::GenerateEnemies()
{
	  const uint32_t minX = 10;      
    const uint32_t maxX = 120;   
    const uint32_t minY = 2;    
    const uint32_t maxY = 18;    
    for (int i = 0; i < enemyCount; ++i) {

        uint32_t randomX = minX + (std::rand() % (maxX - minX + 1)); 
        uint32_t randomY = minY + (std::rand() % (maxY - minY + 1));

        Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(randomX, randomY);

        // 创建敌人对象并初始化
        Enemy* newEnemy = new Enemy();
        newEnemy->Initialize(&viewProjection_, enemyPosition);

        enemies_.push_back(newEnemy);
    }
}
GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete debugCamera_;
	delete skydomeObj_;
	delete player_;
	 for (Enemy* enemy : enemies_) {
        delete enemy;
    }
    enemies_.clear();
	delete cameraController_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
		delete mapChipField_;
		delete deathParticles_;
}

void GameScene::Initialize() {
	std::srand(static_cast<unsigned int>(std::time(nullptr))); 
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	model_ = Model::Create();
	viewProjection_.Initialize();
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);
	skydomeObj_ = new Skydome();
	skydomeObj_->Initialize(&viewProjection_);

	 //Map
	 mapChipField_ = new MapChipField;
	 mapChipField_->LoadMapChipCsv("Resources/map.csv");
	 GenerateBlocks();

	 //Player
	player_ = new Player();
	Vector3 playerPos = mapChipField_->GetMapChipPositionByIndex(2, 17);
	 player_->Initialize(&viewProjection_,playerPos);
	 player_->SetMapChipField(mapChipField_);

	 //敵
GenerateEnemies();
	// Particles
	deathParticles_ = new DeathParticles();
	deathParticles_->Initalize(&viewProjection_);

	  // CameraControll
	cameraController_ = new CameraController;
	cameraController_->Initialize();
	cameraController_->SetTarget(player_); // 追従したいターゲット
	cameraController_->Reset();               // 最初のカメラの位置を追従してるターゲットに設定していく
	Vector3 mapMaxArea = mapChipField_->GetMapChipPositionByIndex(mapChipField_->kNumBlockHorizontal, 0);
	Rect cameraArea = {35, mapMaxArea.x - 37, mapMaxArea.y - 19, 19};
	cameraController_->SetMovableArea(cameraArea);
}

void GameScene::Update() {
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_SPACE)) {
		isDebugCameraActrive_ = !isDebugCameraActrive_;
	}
#endif // _DEBUG
	if (isDebugCameraActrive_) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	} else {
		cameraController_->Update();
		viewProjection_.matView = cameraController_->GetViewProjection().matView;
		viewProjection_.matProjection = cameraController_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	}

	// Block
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			worldTransformBlock->UpdateMatrix();
		}
	}
	CheckAllCollisions();
	// Obj
	skydomeObj_->Update();
	player_->Update();

	// パーティクルの更新
	if (deathParticles_) {
		deathParticles_->Update();
	}
	 for (Enemy* enemy : enemies_) {
        enemy->Update();
    }
	 if (deathParticles_->GetParticlesOver())
		isSceneOver = true;
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			model_->Draw(*worldTransformBlock, viewProjection_);
		}
	}
	skydomeObj_->Draw();
	player_->Draw();
	 for (Enemy* enemy : enemies_) {
        enemy->Draw();
    }
	 //パーティクル描画
	if (deathParticles_) {
		deathParticles_->Draw();
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckAllCollisions()
{ if (player_->GetDead()) {
        return; 
    }
  const float groundY = mapChipField_->GetMapChipPositionByIndex(0, mapChipField_->kNumBlockVirtical - 1).y;
	#pragma region 自キャラと敵キャラの当たり判定
	//判定対象1と2の座標
	AABB aabb1,aabb2;
	//自キャラの座標
    aabb1 = player_->GetAABB();
	//自キャラと敵弾全ての当たり判定
	for (Enemy* enemy : enemies_) {
		///敵弾の座標
		aabb2 = enemy->GetAABB();
		if (IsCollision(aabb1, aabb2)) {
			//自キャラの衝突時コールバックを呼び出す
			player_->OnCollision(true);
			deathParticles_->SetStartPos(player_->GetWorldPosition());
			deathParticles_->SetIsStart(true);
			break;
		}

	}
	if (player_->GetWorldPosition().y < groundY) {

	deathParticles_->SetStartPos(player_->GetWorldPosition());
			deathParticles_->SetIsStart(true);

	}

	#pragma endregion
}
