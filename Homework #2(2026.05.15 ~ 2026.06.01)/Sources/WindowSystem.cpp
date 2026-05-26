#include "Precompiled.h"
#include "WindowSystem.h"

WindowSystem::~WindowSystem() noexcept
{
	Release();
}

bool WindowSystem::Initialize(const WindowOptions& options_)
{
	const HINSTANCE instanceHandle{ ::GetModuleHandleW(nullptr) };

	WNDCLASSEXW windowClass{};
	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = &WindowSystem::WindowProc;
	windowClass.hInstance = instanceHandle;
	windowClass.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
	windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	windowClass.lpszClassName = ClassName;

	if (0 == ::RegisterClassExW(&windowClass))
	{
		const DWORD error{ ::GetLastError() };
		if (ERROR_CLASS_ALREADY_EXISTS != error)
		{
			return false;
		}
	}

	return nullptr != CreateWindow(options_);
}

void WindowSystem::Release()
{
	if (mainWindow)
	{
		mainWindow->Release();
		mainWindow.reset();
	}

	::UnregisterClassW(ClassName, ::GetModuleHandleW(nullptr));
}

Window* WindowSystem::CreateWindow(const WindowOptions& options_)
{
	if (mainWindow)
	{
		return mainWindow.get();
	}

	std::unique_ptr<Window> window{ std::make_unique<Window>() };
	if (!window->Initialize(options_))
	{
		return nullptr;
	}

	mainWindow = std::move(window);
	return mainWindow.get();
}

void WindowSystem::DestroyWindow(Window*& window_) noexcept
{
	if (nullptr == window_ || !mainWindow || window_ != mainWindow.get())
	{
		return;
	}

	mainWindow->Release();
	mainWindow.reset();
	window_ = nullptr;
}

Window& WindowSystem::GetMainWindow() noexcept
{
	assert(mainWindow);
	return *mainWindow;
}

const Window& WindowSystem::GetMainWindow() const noexcept
{
	assert(mainWindow);
	return *mainWindow;
}

LRESULT CALLBACK WindowSystem::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		return window->OnEvent(uMsg, wParam, lParam);
	}

	return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
}
