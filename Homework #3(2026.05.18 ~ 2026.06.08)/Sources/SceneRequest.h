#pragma once

#include <optional>
#include <string>
#include <string_view>

class SceneRequest final
{
public:
	void LoadScene(std::wstring_view sceneName_);
	void UnloadScene() noexcept;

	[[nodiscard]] std::wstring_view GetPendingSceneLoad() const noexcept;
	[[nodiscard]] bool HasSceneChangeRequest() const noexcept;
	[[nodiscard]] bool IsUnloadRequested() const noexcept;

	void Clear() noexcept;

private:
	std::optional<std::wstring> pendingSceneLoad;
	bool unloadRequested{ false };
};
