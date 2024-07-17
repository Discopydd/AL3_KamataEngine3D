#pragma once
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <algorithm>
#include <array>
#include <cmath>
#include"MyMath.h"
///
///
/// 
class  DeathParticles
{
private:
	static inline const uint32_t kNumParticles = 8;
	std::array<WorldTransform, kNumParticles> worldTransforms_;
	ViewProjection* viewProjection_ = nullptr;
	Model* model_ = nullptr;


	
	static inline const float kDuration = 1.5f;                              // 存続時間<秒>
	static inline const float kSpeed = 0.1f;                              // スピード
	static inline const float kAngleUnit = acosf(-1) * 2 / kNumParticles; // 分割下1個分の角度

	bool isFinished_= false;
	float counter_ = 0;
	bool isStart = false;

	ObjectColor objectColor_; // 色変更オブジェクト
	Vector4 color_;           // 色

public:
	~DeathParticles();
	void Initalize(ViewProjection* viewProjection);
	void Update();
	void Draw();


	void SetIsStart(bool flag) { isStart = flag;  };
	void SetStartPos(Vector3 pos);
	const bool GetParticlesOver();
};
