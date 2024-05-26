#include "Skydome.h"

Skydome::~Skydome() { delete _model; }

void Skydome::Initialize(ViewProjection* view) {
	_worldTransform.Initialize();
	_viewProjection = view;
	_model = Model::CreateFromOBJ("Skydome", true);

	_worldTransform.scale_ = {100, 100, 100};
}

void Skydome::Update() { _worldTransform.UpdateMatrix(); }

void Skydome::Draw() { _model->Draw(_worldTransform, *_viewProjection); }
