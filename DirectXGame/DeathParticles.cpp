#include "DeathParticles.h"

DeathParticles::~DeathParticles()
{
	delete model_;
}

void DeathParticles::Initalize(ViewProjection* viewProjection)
{
	model_ = Model::CreateFromOBJ("deathParticle", true);
	viewProjection_ = viewProjection;
	for (auto& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.scale_ = { 2.0f, 2.0f, 2.0f };
	}
	objectColor_.Initialize();
	color_ = {1, 1, 1, 1};
}

void DeathParticles::Update()
{
	if (isFinished_|| !isStart) {
		return;
	}

	for (uint32_t i = 0; i < kNumParticles; ++i) {
		// 基本となる速度ベクトル
		Vector3 velocity = {kSpeed, 0.0f, 0.0f};
		// 回転角を設定する
		float angle = kAngleUnit * i;
		//Z軸周り回転行列
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);
		//基本ベクトルを回転させて速度ベクトルを得る
		velocity = Transform(velocity,matrixRotation);
		//移動処理
		worldTransforms_[i].translation_.x += velocity.x;
		worldTransforms_[i].translation_.y += velocity.y;
	}
	//カウンターを1フレーム分の秒数進める
	
    //存続時間の上限に達したら
	if (counter_ <= kDuration) {
		counter_ += 1.0f / 60.0f;
	}else{
		//終了扱いにする
		isFinished_ = true;
	}
	for (auto& worldTransform : worldTransforms_) {

		worldTransform.UpdateMatrix();
	}

	color_.w = std::clamp(1.0f - counter_ / kDuration, 0.0f, 1.0f);
	//色変更オブジェクトに色の数値を設定する
	objectColor_.SetColor(color_);
	//色変更オブジェクトをVRAMに転送
	objectColor_.TransferMatrix();
}

void DeathParticles::Draw()
{
	if (isFinished_|| !isStart) {
		return;
	}
	for (auto& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, *viewProjection_, &objectColor_);
	}
}

void DeathParticles::SetStartPos(Vector3 pos)
{
	for (auto& worldTransform : worldTransforms_) {
		worldTransform.translation_ = pos;
	}
}

const bool DeathParticles::GetParticlesOver()
{
if (isFinished_)
		return true;
	return false;
}
