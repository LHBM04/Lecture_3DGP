#pragma once

#include "Component.h"

class Camera;

class TitleSceneController final : public Component
{
public:
	TitleSceneController() = default;
	~TitleSceneController() override = default;

	void OnUpdate(float deltaTime_) override;

	void SetCamera(Camera* camera_) noexcept;

private:
	Camera* camera{ nullptr };
};

