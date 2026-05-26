#pragma once

#include <optional>
#include <string>

class InputSystem;
struct ID3D12Device;

class SceneContext final
{
public:
	void RequestSceneChange(std::wstring sceneName_);

	[[nodiscard]] bool HasSceneChangeRequest() const noexcept;
	[[nodiscard]] const std::wstring& GetRequestedSceneName() const;
	void ClearSceneChangeRequest() noexcept;

	void SetInputSystem(InputSystem* inputSystem_) noexcept;
	[[nodiscard]] InputSystem* GetInputSystem() const noexcept;

	void SetDevice(ID3D12Device* device_) noexcept;
	[[nodiscard]] ID3D12Device* GetDevice() const noexcept;

private:
	std::optional<std::wstring> requestedSceneName;
	InputSystem* inputSystem{ nullptr };
	ID3D12Device* device{ nullptr };
};
