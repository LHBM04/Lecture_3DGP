#pragma once

#include "Event.h"
#include "WindowOptions.h"

class Window final
{
public:
	Window() noexcept = default;
	~Window() noexcept = default;

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;

	bool Initialize(const WindowOptions& options_);
	void Release();

	[[nodiscard]] bool ProcessMessages() const noexcept;

	[[nodiscard]] HWND GetHandle() const noexcept;

	[[nodiscard]] const std::wstring& GetTitle() const noexcept;
	void SetTitle(const std::wstring& title_) noexcept;

	[[nodiscard]] int GetX() const noexcept;
	void SetX(int x_) noexcept;

	[[nodiscard]] int GetY() const noexcept;
	void SetY(int y_) noexcept;

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

	void Show() const noexcept;
	void Hide() const noexcept;

	bool PollEvent(Event& event_);

private:
	static LRESULT CALLBACK WindowProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam);

	LRESULT OnEvent(
		UINT message_,
		WPARAM wParam_,
		LPARAM lParam_);

	WindowOptions options;
	HWND handle{ nullptr };
	std::queue<Event> eventQueue;
};
