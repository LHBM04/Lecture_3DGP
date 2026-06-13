#pragma once

#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef IsMinimized
#undef IsMaximized

class WindowSystem;

class Window final
{
public:
	struct Options final
	{
		std::wstring title;
		int x;
		int y;
		int width;
		int height;
		bool isFullscreen;
		bool isResizable;
		bool isBorderless;
	};

	Window() noexcept = default;
	~Window() noexcept = default;

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;

	bool Initialize(const Options& options_);
	void Release() noexcept;

	void Show() noexcept;
	void Hide() noexcept;

	[[nodiscard]] HWND GetHWND() const noexcept;

	[[nodiscard]] const std::wstring& GetTitle() const noexcept;
	void SetTitle(std::wstring title_) noexcept;

	[[nodiscard]] int GetWidth() const noexcept;
	void SetWidth(int width_) noexcept;

	[[nodiscard]] int GetHeight() const noexcept;
	void SetHeight(int height_) noexcept;

	[[nodiscard]] SIZE GetSize() const noexcept;
	void SetSize(SIZE size_) noexcept;

	[[nodiscard]] int GetX() const noexcept;
	void SetX(int x_) noexcept;
	
	[[nodiscard]] int GetY() const noexcept;
	void SetY(int y_) noexcept;

	[[nodiscard]] POINT GetPosition() const noexcept;
	void SetPosition(POINT position_) noexcept;

	[[nodiscard]] bool IsFullscreen() const noexcept;
	void SetFullscreen(bool fullscreen_) noexcept;

	[[nodiscard]] bool IsResizable() const noexcept;
	void SetResizable(bool resizable_) noexcept;

	[[nodiscard]] bool IsBorderless() const noexcept;
	void SetBorderless(bool decorated_) noexcept;

	[[nodiscard]] bool IsMinimized() const noexcept;
	void Minimize() noexcept;

	[[nodiscard]] bool IsMaximized() const noexcept;
	void Maximize() noexcept;

private:
	Options options;
	HWND hWnd;
};
