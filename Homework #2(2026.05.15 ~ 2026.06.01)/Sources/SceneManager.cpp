#include "Precompiled.h"
#include "SceneManager.h"

#include "Application.h"
#include "SceneSystem.h"

void SceneManager::LoadScene(std::size_t index_) noexcept
{
	Application::GetSceneSystem().LoadScene(index_);
}

void SceneManager::LoadScene(const std::wstring& name_) noexcept
{
	Application::GetSceneSystem().LoadScene(name_);
}
