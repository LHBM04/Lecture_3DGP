#pragma once

#include "Requests.h"

class Scene
{
public:
	Scene() = default;
	virtual ~Scene() = default;

	virtual void Update(const TimeContext& time_);
	virtual void Render(RenderContext& renderContext_);

	[[nodiscard]] ApplicationRequest* GetApplicationRequest() const noexcept;
	[[nodiscard]] SceneRequest* GetSceneRequest() const noexcept;
	void SetApplicationRequest(ApplicationRequest* applicationRequest_) noexcept;
	void SetSceneRequest(SceneRequest* sceneRequest_) noexcept;

private:
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	Scene(Scene&&) = delete;
	Scene& operator=(Scene&&) = delete;

	ApplicationRequest* applicationRequest{ nullptr };
	SceneRequest* sceneRequest{ nullptr };
};
