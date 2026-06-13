#include "Precompiled.h"
#include "WindowService.h"

namespace
{
	// 윈도우 프로시저.
	LRESULT CALLBACK WindowProc(
		_In_ HWND hWnd,
		_In_ UINT uMsg,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_DESTROY:
		{
			::PostQuitMessage(0);
			return 0;
		}
		default:
			return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
		}

		::std::unreachable();
	}
}

Window* WindowService::AddWindow(const Window::Options& options_)
{
	auto window{ std::make_unique<Window>() };
	if (!window->Initialize(options_))
	{
		return nullptr;
	}

	return window.get();
}

void WindowService::RemoveWindow(Window* window_)
{
	std::erase_if(windows, [window_](const auto& window)
		{
			return window.get() == window_;
		});
}

void WindowService::OnAdd()
{
	// TODO: 이거 여부 확인할 수 있는 API 있으니까 대체.
	// 일단은 이미 hInstance가 있으면 초기화된 걸로 간주.
	if (hInstance != nullptr)
	{
		return;
	}

	// hInstance 획득.
	// 인자로 받는 것보다 이게 더 깔끔함.
	hInstance = ::GetModuleHandleW(nullptr);

	// WNDCLASSEXW 초기화.
	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = ::LoadIconW(hInstance, IDI_APPLICATION);
	wc.hCursor = ::LoadCursorW(hInstance, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = WindowClassName;
	wc.hIconSm = ::LoadIconW(hInstance, IDI_APPLICATION);
	::RegisterClassExW(&wc);
}

void WindowService::OnRemove()
{
	::UnregisterClassW(WindowClassName, hInstance);
	hInstance = nullptr;
}
