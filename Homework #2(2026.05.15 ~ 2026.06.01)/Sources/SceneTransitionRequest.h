#pragma once

#include <string>

class SceneTransitionRequest
{
public:
	virtual ~SceneTransitionRequest() = default;

	[[nodiscard]] virtual bool HasSceneTransitionRequest() const noexcept = 0;
	[[nodiscard]] virtual const std::wstring& GetTargetSceneName() const noexcept = 0;
	virtual void ClearSceneTransitionRequest() noexcept = 0;
};
