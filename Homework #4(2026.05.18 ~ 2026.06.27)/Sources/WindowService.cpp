#include "Precompiled.h"
#include "WindowService.h"

#include "Framework.h"

LRESULT CALLBACK WindowProc(
	_In_ HWND hWnd, 
	_In_ UINT uMsg, 
	_In_ WPARAM wParam, 
	_In_ LPARAM lParam)
{
	Window* window{ nullptr };

	if (uMsg == WM_NCCREATE)
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		window = reinterpret_cast<Window*>(pCreate->lpCreateParams);
		::SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
	}
	else
	{
		window = reinterpret_cast<Window*>(::GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	}

	if (window != nullptr)
	{
		return window->HandleMessage(hWnd, uMsg, wParam, lParam);
	}

	return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

Window* WindowService::AddWindow(const WindowOptions& options_)
{
	std::unique_ptr<Window> newWindow{ std::make_unique<Window>() };
	if (!newWindow->Initialize(options_))
	{
		return nullptr;
	}

	return windows.emplace_back(std::move(newWindow)).get();
}

void WindowService::RemoveWindow(Window& window_)
{
	std::erase_if(windows, [&window_](const std::unique_ptr<Window>& window)
		{
			return window.get() == &window_;
		});
}

Window* WindowService::GetMainWindow() const noexcept
{
	return mainWindow;
}

void WindowService::SetMainWindow(Window& window_) noexcept
{
	mainWindow = &window_;
}

bool WindowService::PollEvents()
{
	MSG msg;
	while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return true;
}

void WindowService::OnAdd()
{
	HINSTANCE hInstance{ GetFramework().GetOption<HINSTANCE>(L"App.Instance") };

	WNDCLASSEXW windowClass{};
	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = ::LoadIconW(hInstance, IDI_APPLICATION);
	windowClass.hCursor = ::LoadCursorW(hInstance, IDC_ARROW);
	windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = L"Homework #3 Class";
	windowClass.hIconSm = ::LoadIconW(hInstance, IDI_APPLICATION);
	::RegisterClassExW(&windowClass);
}

void WindowService::OnRemove()
{
	HINSTANCE hInstance{ GetFramework().GetOption<HINSTANCE>(L"App.Instance") };
	::UnregisterClassW(L"Homework #3 Class", hInstance);
}
