#pragma once

#include "Collidable.h"
#include "Component.h"
#include "SceneTransitionRequest.h"

class ScenePortal final : public Component, public Collidable, public SceneTransitionRequest
{
public:
	ScenePortal() = default;
	~ScenePortal() override = default;

	void SetTargetSceneName(std::wstring sceneName_);
	[[nodiscard]] const std::wstring& GetTargetSceneName() const noexcept override;

	[[nodiscard]] bool HasSceneTransitionRequest() const noexcept override;
	void ClearSceneTransitionRequest() noexcept override;

	[[nodiscard]] bool WantsCollisionEvents() const noexcept override;

	void OnCollisionEnter(GameObject& other_) override;
	void OnCollisionStay(GameObject& other_) override;
	void OnCollisionExit(GameObject& other_) override;

private:
	std::wstring targetSceneName;
	bool requested{ false };
};
