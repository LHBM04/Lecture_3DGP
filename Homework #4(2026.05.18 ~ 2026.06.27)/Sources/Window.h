#pragma once

#include <utility>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class Window
{
public:
	Window() noexcept = default;
	~Window() noexcept = default;

	[[nodiscard]] HWND GetHWND() const noexcept;

	[[nodiscard]] LPCWSTR GetTitle() const noexcept;
	void SetTitle(LPCWSTR title_) noexcept;

	[[nodiscard]] UINT GetWidth() const noexcept;
	void SetWidth(UINT width_) noexcept;

	[[nodiscard]] UINT GetHeight() const noexcept;
	void SetHeight(UINT height_) noexcept;

	[[nodiscard]] std::pair<UINT, UINT> GetSize() const noexcept;
	void SetSize(UINT width_, UINT height_) noexcept;

	[[nodiscard]] UINT GetPositionX() const noexcept;
	void SetPositionX(UINT x_) noexcept;

	[[nodiscard]] UINT GetPositionY() const noexcept;
	void SetPositionY(UINT y_) noexcept;

	[[nodiscard]] std::pair<UINT, UINT> GetPosition() const noexcept;
	void SetPosition(UINT x_, UINT y_) noexcept;

private:
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;

	HWND hWnd{ nullptr };
};
