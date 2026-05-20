#pragma once

class EventQueue;
class Window;
struct WindowOptions;

class WindowSystem
{
	STATIC_CLASS(WindowSystem);

	[[nodiscard]] static bool Initialize(HINSTANCE instance_) noexcept;
	static void Release() noexcept;

	[[nodiscard]] static std::unique_ptr<Window> Create(const WindowOptions& options) noexcept;

	[[nodiscard]] static bool PollEvents(EventQueue& eventQueue_) noexcept;

private:
	[[nodiscard]] static LRESULT CALLBACK WindowProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam) noexcept;

	static inline HINSTANCE instance = nullptr;
	static inline const wchar_t* className = L"Homework #2 Class";
	static inline EventQueue* currentEventQueue = nullptr;
};
