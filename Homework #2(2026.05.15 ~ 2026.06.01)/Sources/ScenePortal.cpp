#include "Precompiled.h"
#include "ScenePortal.h"

#include "GameObject.h"

void ScenePortal::SetTargetSceneName(std::wstring sceneName_)
{
	targetSceneName = std::move(sceneName_);
}

const std::wstring& ScenePortal::GetTargetSceneName() const noexcept
{
	return targetSceneName;
}

bool ScenePortal::HasSceneTransitionRequest() const noexcept
{
	return requested && !targetSceneName.empty();
}

void ScenePortal::ClearSceneTransitionRequest() noexcept
{
	requested = false;
}

bool ScenePortal::WantsCollisionEvents() const noexcept
{
	return true;
}

void ScenePortal::OnCollisionEnter(GameObject& other_)
{
	if ("Player" == other_.GetTag())
	{
		requested = true;
	}
}

void ScenePortal::OnCollisionStay(GameObject& other_)
{
	(void)other_;
}

void ScenePortal::OnCollisionExit(GameObject& other_)
{
	(void)other_;
}
