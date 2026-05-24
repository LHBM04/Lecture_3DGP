#pragma once

#include <memory>
#include <string>

class Scene;

namespace SceneManager
{
	void Update();
	void Render();

	void AddScene(const std::wstring& name_, std::unique_ptr<Scene> scene_);
	void RemoveScene(const std::wstring& name_);

	void LoadScene(std::size_t index_) noexcept;
	void UnloadScene(std::size_t index_) noexcept;
	void LoadScene(const std::wstring& name_) noexcept;
	void UnloadScene(const std::wstring& name_) noexcept;

	[[nodiscard]] Scene* GetCurrentScene() noexcept;
}
