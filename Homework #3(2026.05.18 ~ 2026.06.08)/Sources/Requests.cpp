#include "Precompiled.h"
#include "Requests.h"

void ApplicationRequest::QuitGame() noexcept
{
	quitRequested = true;
}

bool ApplicationRequest::IsQuitRequested() const noexcept
{
	return quitRequested;
}

void ApplicationRequest::Clear() noexcept
{
	quitRequested = false;
}

void SceneRequest::LoadScene(std::wstring_view sceneName_)
{
	pendingSceneLoad = std::wstring(sceneName_);
	unloadRequested = false;
}

void SceneRequest::UnloadScene() noexcept
{
	pendingSceneLoad.reset();
	unloadRequested = true;
}

std::wstring_view SceneRequest::GetPendingSceneLoad() const noexcept
{
	if (!pendingSceneLoad.has_value())
	{
		return {};
	}

	return *pendingSceneLoad;
}

bool SceneRequest::HasSceneChangeRequest() const noexcept
{
	return pendingSceneLoad.has_value() || unloadRequested;
}

bool SceneRequest::IsUnloadRequested() const noexcept
{
	return unloadRequested;
}

void SceneRequest::Clear() noexcept
{
	pendingSceneLoad.reset();
	unloadRequested = false;
}

void RenderContext::Submit(const RenderRequest& request_)
{
	requests.push_back(request_);
}

const std::vector<RenderRequest>& RenderContext::GetRequests() const noexcept
{
	return requests;
}

void RenderContext::Clear() noexcept
{
	requests.clear();
}
