#include "Audio.h"
#include "AxisIndicator.h"
#include "DirectXCommon.h"
#include "GameScene.h"
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include "TitleScene.h"
#include "WinApp.h"
#include "Sence.h" 


Scene scene = Scene::kUnknown;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	// 汎用機能
	Input* input = nullptr;
	Audio* audio = nullptr;
	AxisIndicator* axisIndicator = nullptr;
	PrimitiveDrawer* primitiveDrawer = nullptr;
	GameScene* gameScene = nullptr;
	TitleScene* titleScene = nullptr;

	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow(L"KNIGHT ADVENT");

	// DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

#pragma region 汎用機能初期化
	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize();

	// オーディオの初期化
	audio = Audio::GetInstance();
	audio->Initialize();

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());
	TextureManager::Load("white1x1.png");

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 3Dモデル静的初期化
	Model::StaticInitialize();

	// 軸方向表示初期化
	axisIndicator = AxisIndicator::GetInstance();
	axisIndicator->Initialize();

	primitiveDrawer = PrimitiveDrawer::GetInstance();
	primitiveDrawer->Initialize();
#pragma endregion

	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize();
	scene = Scene::kLoading;

	// メインループ
	while (true) {
		// メッセージ処理
		if (win->ProcessMessage()) {
			break;
		}

		switch (scene) {
		case Scene::kUnknown:
			break;
		case Scene::kLoading:
			// ゲームシーンの初期化
			delete gameScene;
			delete titleScene;
			gameScene = new GameScene();
			gameScene->Initialize();
			titleScene = new TitleScene();
			titleScene->Initalize();
			scene = Scene::kTitle;
			break;
		case Scene::kTitle:
			// ImGui受付開始
			imguiManager->Begin();
			// 入力関連の毎フレーム処理
			input->Update();
			// ゲームシーンの毎フレーム処理
			titleScene->Update();
			// 軸表示の更新
			axisIndicator->Update();
			// ImGui受付終了
			imguiManager->End();

			// 描画開始
			dxCommon->PreDraw();
			// ゲームシーンの描画
			titleScene->Draw();
			// 軸表示の描画
			axisIndicator->Draw();
			// プリミティブ描画のリセット
			primitiveDrawer->Reset();
			// ImGui描画
			imguiManager->Draw();
			// 描画終了
			dxCommon->PostDraw();

			// Scene Change
			if (Input::GetInstance()->PushKey(DIK_SPACE))
				scene = Scene::kGame;
			break;
		case Scene::kGame:
			// ImGui受付開始
			imguiManager->Begin();
			// 入力関連の毎フレーム処理
			input->Update();
			// ゲームシーンの毎フレーム処理
			gameScene->Update();
			// 軸表示の更新
			axisIndicator->Update();
			// ImGui受付終了
			imguiManager->End();

			// 描画開始
			dxCommon->PreDraw();
			// ゲームシーンの描画
			gameScene->Draw();
			// 軸表示の描画
			axisIndicator->Draw();
			// プリミティブ描画のリセット
			primitiveDrawer->Reset();
			// ImGui描画
			imguiManager->Draw();
			// 描画終了
			dxCommon->PostDraw();

			// SceneChange
			if (gameScene->isSceneOver)
				scene = Scene::kLoading;
			break;
		}
	}

	// 各種解放
	delete gameScene;

	delete titleScene;
	// 3Dモデル解放
	Model::StaticFinalize();
	audio->Finalize();
	// ImGui解放
	imguiManager->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}