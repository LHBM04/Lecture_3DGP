#pragma once

#include "System.h"
#include "Window.h"

#ifdef CreateWindow
#undef CreateWindow
#endif

class WindowSystem : public ISystem
{
public:
	bool Initialize();
	void Release() override;

	[[nodiscard]] Window* CreateWindow(const Window::Options& options_);
	void DestroyWindow(Window* window_);

	std::span<std::unique_ptr<Window>> GetWindows()
	{
		return windows;
	}

private:
	static LRESULT CALLBACK WindowProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam);

	std::vector<std::unique_ptr<Window>> windows;
};
