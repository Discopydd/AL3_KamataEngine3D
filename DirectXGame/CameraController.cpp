#include "CameraController.h"

void CameraController::Initialize()
{
	viewProjection_.Initialize();
}

void CameraController::Update()
{
    // 追従対象のワールドトランスフォームを参照
        const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
        // 追従対象とオフセットからカメラの座標を計算
       targetPos_ = targetWorldTransform.translation_ + targetOffset_ + (target_->GetVelocity() * kVelocityBias);


       viewProjection_.translation_.x = std::lerp(viewProjection_.translation_.x, targetPos_.x, kInterpolationRate);
        viewProjection_.translation_.y = std::lerp(viewProjection_.translation_.y, targetPos_.y, kInterpolationRate);
        viewProjection_.translation_.z = std::lerp(viewProjection_.translation_.z, targetPos_.z, kInterpolationRate);

        viewProjection_.translation_.x = std::clamp(viewProjection_.translation_.x, targetPos_.x + targetArea_.left, targetPos_.x + targetArea_.right);
        viewProjection_.translation_.y = std::clamp(viewProjection_.translation_.y, targetPos_.y + targetArea_.bottom, targetPos_.y + targetArea_.top);

        viewProjection_.translation_.x = std::clamp(viewProjection_.translation_.x, movableArea_.left, movableArea_.right);
        viewProjection_.translation_.y = std::clamp(viewProjection_.translation_.y, movableArea_.bottom, movableArea_.top);

        viewProjection_.UpdateMatrix();
}

void CameraController::Reset()
{
	  // 追従対象のワールドトランスフォームを参照
        const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
        // 追従対象とオフセットからカメラの座標を計算
        viewProjection_.translation_ = targetWorldTransform.translation_ + targetOffset_;
}
