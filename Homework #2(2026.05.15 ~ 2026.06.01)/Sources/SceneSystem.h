#pragma once

#include "System.h"
#include "Scene.h"

class TimeContext;
class RenderContext;

class SceneSystem : public ISystem
{
public:
	bool Initialize();
	void Release() override;

	void Update(const TimeContext& context_);
	void Render(RenderContext& context_);

	[[nodiscard]] Scene* GetCurrentScene() noexcept;
	[[nodiscard]] const Scene* GetCurrentScene() const noexcept;

private:
	std::unordered_map<std::wstring, std::unique_ptr<Scene>> scenes;
};
