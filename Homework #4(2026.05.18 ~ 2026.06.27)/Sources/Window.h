#pragma once

#include <utility>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class WindowOptions final
{
public:
	HINSTANCE hInstance;
	LPCWSTR title;
	int x;
	int y;
	int width;
	int height;
	DWORD style;
	DWORD styleEx;
};

class Window
{
public:
	Window() noexcept = default;
	~Window() noexcept = default;

	bool Initialize(const WindowOptions& options);
	void Release();

	void Show(int show_) const;
	void Hide() const;

	[[nodiscard]] HWND GetHWND() const noexcept;

	[[nodiscard]] LPCWSTR GetTitle() const noexcept;
	void SetTitle(LPCWSTR title_) noexcept;

	[[nodiscard]] UINT GetWidth() const noexcept;
	void SetWidth(UINT width_) noexcept;

	[[nodiscard]] UINT GetHeight() const noexcept;
	void SetHeight(UINT height_) noexcept;

	[[nodiscard]] SIZE GetSize() const noexcept;
	void SetSize(SIZE size_) noexcept;

	[[nodiscard]] UINT GetPositionX() const noexcept;
	void SetPositionX(UINT x_) noexcept;

	[[nodiscard]] UINT GetPositionY() const noexcept;
	void SetPositionY(UINT y_) noexcept;

	[[nodiscard]] POINT GetPosition() const noexcept;
	void SetPosition(POINT position_) noexcept;

	LRESULT HandleMessage(HWND hWnd_, UINT message_, WPARAM wParam_, LPARAM lParam_) noexcept;

private:
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;

	HWND hWnd{ nullptr };
	WindowOptions options;
};
