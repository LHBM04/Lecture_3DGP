#pragma once

#include "Component.h"

class Camera;

class MenuSceneController final : public Component
{
public:
	MenuSceneController() = default;
	~MenuSceneController() override = default;

	void OnUpdate(float deltaTime_) override;
	void SetCamera(Camera* camera_) noexcept;

private:
	Camera* camera{ nullptr };
};
