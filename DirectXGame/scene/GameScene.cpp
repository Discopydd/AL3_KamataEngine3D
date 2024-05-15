#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include"MyMath.h"
GameScene::GameScene() {}

GameScene::~GameScene() {
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
        for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
            if (worldTransformBlock) {
                delete worldTransformBlock;
            }
        }
        worldTransformBlockLine.clear();
    }
    worldTransformBlocks_.clear();
	delete debugcamer_;
	
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	modelBlock_ = Model::Create();
	worldTransform_.Initialize();
	viewProjection_.Initialize();

	
	const uint32_t kNumBlockVirtical = 10;
	const uint32_t kNumBlockhorizontal = 20;

	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	   worldTransformBlocks_.resize(kNumBlockVirtical);
    for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
        worldTransformBlocks_[i].resize(kNumBlockhorizontal);
        for (uint32_t j = 0; j < kNumBlockhorizontal; ++j) {
            if ((i + j) % 2 == 0) {
                worldTransformBlocks_[i][j] = new WorldTransform();
                worldTransformBlocks_[i][j]->Initialize();
                worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
                worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
            } else {
                worldTransformBlocks_[i][j] = nullptr;
            }
        }
    }
	debugcamer_ = new DebugCamera(1280, 720);
}

void GameScene::Update() {
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_,worldTransformBlock->rotation_,worldTransformBlock->translation_);
			worldTransformBlock->TransferMatrix();
		}
	}
#ifdef  _DEBUG
	if (input_->TriggerKey(DIK_SPACE)) {
		isDebugCamerActive_ = true;
	}
#endif //  _DEBUG
	if (isDebugCamerActive_) {
		debugcamer_->Update();
		viewProjection_.matView = debugcamer_->GetViewProjection().matView;
		viewProjection_.matProjection = debugcamer_->GetViewProjection().matProjection;
			viewProjection_.TransferMatrix();
	}
	else {
		viewProjection_.UpdateMatrix();
	}

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
			modelBlock_->Draw(*worldTransformBlock, viewProjection_);
		}
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
