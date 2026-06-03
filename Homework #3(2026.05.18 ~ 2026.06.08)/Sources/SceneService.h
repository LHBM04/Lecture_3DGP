#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Requests.h"
#include "Scene.h"
#include "Service.h"

class SceneService final : public Service
{
public:
	void AddScene(std::wstring_view name_, std::unique_ptr<Scene> scene_);
	void RemoveScene(std::wstring_view name_);

	void LoadScene(std::wstring_view name_);
	void UnloadScene();

	void Update(const TimeContext& time_);
	void Render(RenderContext& renderContext_);

	void SetRequests(ApplicationRequest& applicationRequest_, SceneRequest& sceneRequest_) noexcept;
	void SetSceneRequest(SceneRequest& sceneRequest_) noexcept;
	[[nodiscard]] ApplicationRequest* GetApplicationRequest() const noexcept;
	[[nodiscard]] SceneRequest* GetSceneRequest() const noexcept;

protected:
	void OnAdd() override;
	void OnRemove() override;

private:
	std::unordered_map<std::wstring, std::unique_ptr<Scene>> scenes;
	ApplicationRequest* applicationRequest{ nullptr };
	SceneRequest* sceneRequest{ nullptr };
	Scene* currentScene{ nullptr };
};
