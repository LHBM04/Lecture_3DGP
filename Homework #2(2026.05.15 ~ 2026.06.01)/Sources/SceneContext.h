#pragma once

#include <optional>
#include <string>

class SceneContext final
{
public:
	void RequestSceneChange(std::wstring sceneName_);

	[[nodiscard]] bool HasSceneChangeRequest() const noexcept;
	[[nodiscard]] const std::wstring& GetRequestedSceneName() const;
	void ClearSceneChangeRequest() noexcept;

private:
	std::optional<std::wstring> requestedSceneName;
};
