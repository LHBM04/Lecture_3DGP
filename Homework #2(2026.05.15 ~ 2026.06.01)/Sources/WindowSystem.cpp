#include "Precompiled.h"
#include "WindowSystem.h"

bool WindowSystem::Initialize()
{
	const HINSTANCE instanceHandle{ ::GetModuleHandleW(nullptr) };

	WNDCLASSEXW windowClass{};
	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = &WindowSystem::WindowProc;
	windowClass.hInstance = instanceHandle;
	windowClass.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
	windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	windowClass.lpszClassName = L"Engine";

	if (0 == ::RegisterClassExW(&windowClass))
	{
		const DWORD error{ ::GetLastError() };
		if (ERROR_CLASS_ALREADY_EXISTS != error)
		{
			return false;
		}
	}

	return true;
}

void WindowSystem::Release()
{
	::UnregisterClassW(L"Engine", ::GetModuleHandleW(nullptr));
}

std::expected<Window*, std::wstring> WindowSystem::CreateWindow(const WindowOptions& options_)
{
	std::unique_ptr<Window> newWindow{ std::make_unique<Window>() };
	if (!newWindow->Initialize(options_))
	{
		return std::unexpected{ L"좆됨" };
	}

	windows.push_back(std::move(newWindow));
	return windows.back().get();
}

void WindowSystem::DestroyWindow(Window* window_)
{

}

void WindowSystem::PollEvents(EventQueue& eventQueue_)
{
	for (const std::unique_ptr<Window>& window : windows)
	{
		if (window != nullptr)
		{
			window->currentEventQueue = &eventQueue_;
		}
	}

	MSG message{};
	while (::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&message);
		::DispatchMessageW(&message);
	}

	for (const std::unique_ptr<Window>& window : windows)
	{
		if (window != nullptr)
		{
			window->currentEventQueue = nullptr;
		}
	}
}

std::span<std::unique_ptr<Window>> WindowSystem::GetWindows()
{
	return windows;
}

std::span<const std::unique_ptr<Window>> WindowSystem::GetWindows() const
{
	return windows;
}

LRESULT WindowSystem::WindowProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	Window* window{ nullptr };

	if (uMsg == WM_NCCREATE)
	{
		auto* createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
		window = static_cast<Window*>(createStruct->lpCreateParams);

		::SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
		window->handle = hWnd;
	}
	else
	{
		window = reinterpret_cast<Window*>(::GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	}

	if (window != nullptr)
	{
		return window->ProceedEvent(uMsg, wParam, lParam);
	}

	return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
}
