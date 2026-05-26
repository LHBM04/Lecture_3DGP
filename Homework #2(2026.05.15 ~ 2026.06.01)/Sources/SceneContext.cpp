#include "Precompiled.h"
#include "SceneContext.h"

void SceneContext::RequestSceneChange(std::wstring sceneName_)
{
	// Current policy: the last scene transition request in a frame wins.
	requestedSceneName = std::move(sceneName_);
}

bool SceneContext::HasSceneChangeRequest() const noexcept
{
	return requestedSceneName.has_value();
}

const std::wstring& SceneContext::GetRequestedSceneName() const
{
	return requestedSceneName.value();
}

void SceneContext::ClearSceneChangeRequest() noexcept
{
	requestedSceneName.reset();
}
