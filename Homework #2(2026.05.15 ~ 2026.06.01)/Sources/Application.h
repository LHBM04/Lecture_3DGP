#pragma once

#include "RenderSystem.h"
#include "SceneSystem.h"
#include "WindowSystem.h"

struct ApplicationOptions final
{
	std::wstring title;
	int width;
	int height;
	bool fullscreen;
	bool borderless;
	bool resizable;
};

namespace Application
{
	bool Initialize(const ApplicationOptions& options_);
	
	int Run();
	void Quit();

	Renderer& GetRenderer();
	Window& GetWindow();
	SceneSystem& GetSceneSystem();
}
