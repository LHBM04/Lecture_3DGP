#pragma once

#include <memory>
#include <string>

class Scene;
struct ID3D12Device;

namespace SceneManager
{
	bool Initialize(ID3D12Device* device_) noexcept;
	void Release() noexcept;

	void Update();
	void Render();

	void AddScene(const std::wstring& name_, std::unique_ptr<Scene> scene_);
	void RemoveScene(const std::wstring& name_);

	void LoadScene(std::size_t index_) noexcept;
	void UnloadScene(std::size_t index_) noexcept;
	void LoadScene(const std::wstring& name_) noexcept;
	void UnloadScene(const std::wstring& name_) noexcept;

	[[nodiscard]] Scene* GetCurrentScene() noexcept;

	void SetTransitionDuration(float duration_) noexcept;
}
