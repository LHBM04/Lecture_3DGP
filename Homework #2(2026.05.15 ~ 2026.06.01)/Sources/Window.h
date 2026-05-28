#pragma once

#include "EventQueue.h"
#include "WindowOptions.h"

class Window
{
	friend class WindowSystem;

public:
	Window() = default;
	~Window() = default;

	Window(const Window&) = delete;
	Window operator=(const Window&) = delete;

	Window(Window&&) = delete;
	Window operator=(Window&&) = delete;

	bool Initialize(const WindowOptions& options_);
	void Release();

	void Show() const noexcept;
	void Hide() const noexcept;

	[[nodiscard]] HWND GetHandle() const noexcept;

	[[nodiscard]] const std::wstring& GetTitle() const noexcept;
	void SetTitle(const std::wstring& title_) noexcept;

	[[nodiscard]] int GetPositionX() const noexcept;
	void SetPositionX(int x_) noexcept;

	[[nodiscard]] int GetPositionY() const noexcept;
	void SetPositionY(int y_) noexcept;

	[[nodiscard]] std::pair<int, int> GetPosition() const noexcept;
	void SetPosition(int x_, int y_) noexcept;

	[[nodiscard]] int GetWidth() const noexcept;
	void SetWidth(int width_) noexcept;

	[[nodiscard]] int GetHeight() const noexcept;
	void SetHeight(int height_) noexcept;

	[[nodiscard]] std::pair<int, int> GetSize() const noexcept;
	void SetSize(int width_, int height_);

	[[nodiscard]] DWORD GetStyle() const noexcept;
	void SetStyle(DWORD style_) noexcept;

	[[nodiscard]] DWORD GetStyleEx() const noexcept;
	void SetStyleEx(DWORD styleEx_) noexcept;

	LRESULT ProceedEvent(
		UINT message_,
		WPARAM wParam_,
		LPARAM lParam_);

private:
	WindowOptions options;
	HWND handle{ nullptr };
	EventQueue* currentEventQueue{ nullptr };
};
