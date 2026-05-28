#pragma once

#include "EventQueue.h"
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

	[[nodiscard]] std::expected<Window*, std::wstring> CreateWindow(const WindowOptions& options_);
	void DestroyWindow(Window* window_);
	void PollEvents(EventQueue& eventQueue_);

	[[nodiscard]] std::span<std::unique_ptr<Window>> GetWindows();
	[[nodiscard]] std::span<const std::unique_ptr<Window>> GetWindows() const;

private:
	static LRESULT CALLBACK WindowProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam);

	std::vector<std::unique_ptr<Window>> windows;
	mutable std::vector<Window*> activeWindows;
};
