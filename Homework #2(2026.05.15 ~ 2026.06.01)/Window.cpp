#include "Precompiled.h"
#include "Window.h"
#include "WindowOptions.h"

Window::Window(const WindowOptions& options_, HWND handle_) noexcept
	: options(options_)
	, handle(handle_)
{
}

Window::~Window() noexcept
{
	if (nullptr != handle)
	{
		::DestroyWindow(handle);
		handle = nullptr;
	}
}

[[nodiscard]] HWND Window::GetHandle() const noexcept
{
	return handle;
}

[[nodiscard]] std::wstring Window::GetTitle() const noexcept
{
	return options.title;
}

void Window::SetTitle(const std::wstring& title_) noexcept
{
	options.title = title_;
	::SetWindowTextW(handle, options.title.c_str());
}
