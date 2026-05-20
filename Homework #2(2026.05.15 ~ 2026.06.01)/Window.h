#pragma once

#include "WindowOptions.h"

class Window
{
	friend class WindowSystem;

public:
	~Window() noexcept;

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;

	[[nodiscard]] HWND GetHandle() const noexcept;

	[[nodiscard]] std::wstring GetTitle() const noexcept;
	void SetTitle(const std::wstring& title_) noexcept;

private:
	explicit Window(const WindowOptions& options_, HWND handle_) noexcept;

	WindowOptions options;
	HWND handle;
};
