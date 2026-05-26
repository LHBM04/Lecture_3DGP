#pragma once

#include "System.h"
#include "Window.h"

#include <memory>

#ifdef CreateWindow
#undef CreateWindow
#endif

class WindowSystem final : public System
{
public:
	static constexpr const wchar_t* ClassName{ L"HomeworkWindowClass" };

	~WindowSystem() noexcept override;

	bool Initialize(const WindowOptions& options_);
	void Release() override;

	[[nodiscard]] Window* CreateWindow(const WindowOptions& options_);
	void DestroyWindow(Window*& window_) noexcept;

	[[nodiscard]] Window& GetMainWindow() noexcept;
	[[nodiscard]] const Window& GetMainWindow() const noexcept;

private:
	static LRESULT CALLBACK WindowProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam);

	std::unique_ptr<Window> mainWindow;
};
