#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include"ViewProjection.h"
class Door {
private:
    WorldTransform worldTransform_;

	Model* model_ = nullptr;

	ViewProjection* viewProjection_ = nullptr;

public:
   void Initialize(ViewProjection* viewProjection, const Vector3& position);
   void Update();
    void Draw();
    const WorldTransform& GetWorldTransform() const { return worldTransform_; }
};
