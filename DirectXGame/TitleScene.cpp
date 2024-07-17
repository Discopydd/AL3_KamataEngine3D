#include "TitleScene.h"
TitleScene::~TitleScene() {
	delete modelTitle_;
	delete modelPlayer_;
}

void TitleScene::Initalize() {
	dxCommon_ = DirectXCommon::GetInstance();
	viewProjection_.Initialize();
	worldTransform_title.Initialize();
	worldTransform_player.Initialize();
	modelTitle_ = Model::CreateFromOBJ("Title", true);
	modelPlayer_ = Model::CreateFromOBJ("Player", true);

	worldTransform_title.translation_ = {-20, 5, 0};
	worldTransform_title.scale_ = {10, 10, 10};
	worldTransform_player.translation_ = {0, -10, 0};
	worldTransform_player.scale_ = {3, 3, 3};
	worldTransform_player.rotation_ = {0, 1, 0.5f};
}

void TitleScene::Update() {
	if (currentTime_ < titleMoveTime_)
		currentTime_ += 1 / 60.f;
	else {
		currentTime_ = 0;
		titleSpeed_ *= -1;
	}
	worldTransform_title.translation_ += {0, titleSpeed_, 0};

	worldTransform_player.rotation_ += {0, 0.01f, 0.005f};

	worldTransform_title.UpdateMatrix();
	worldTransform_player.UpdateMatrix();
}

void TitleScene::Draw() {

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
	modelTitle_->Draw(worldTransform_title, viewProjection_);
	modelPlayer_->Draw(worldTransform_player, viewProjection_);

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
