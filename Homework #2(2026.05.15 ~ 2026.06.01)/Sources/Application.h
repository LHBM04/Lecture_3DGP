#pragma once

#include "Renderer.h"
#include "Window.h"

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
}