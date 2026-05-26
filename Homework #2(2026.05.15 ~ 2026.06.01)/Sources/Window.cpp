#include "Precompiled.h"
#include "Window.h"

#include "Application.h"
#include "InputSystem.h"

Window::~Window() noexcept
{
	Window::Release();
}

bool Window::Initialize(const WindowOptions& options_)
{
	if (nullptr != handle)
	{
		return true;
	}

	options = options_;
	options.width = std::max(1, options.width);
	options.height = std::max(1, options.height);

	const HINSTANCE instanceHandle{ ::GetModuleHandleW(nullptr) };
	const wchar_t* className{ L"HomeworkWindowClass" };

	WNDCLASSEXW windowClass{};
	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = &Window::WindowProc;
	windowClass.hInstance = instanceHandle;
	windowClass.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
	windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	windowClass.lpszClassName = className;

	if (0 == ::RegisterClassExW(&windowClass))
	{
		const DWORD error{ ::GetLastError() };
		if (ERROR_CLASS_ALREADY_EXISTS != error)
		{
			return false;
		}
	}

	RECT rect{ 0, 0, options.width, options.height };
	::AdjustWindowRectEx(&rect, options.style, FALSE, options.styleEx);

	const int windowWidth{ rect.right - rect.left };
	const int windowHeight{ rect.bottom - rect.top };
	const int screenWidth{ ::GetSystemMetrics(SM_CXSCREEN) };
	const int screenHeight{ ::GetSystemMetrics(SM_CYSCREEN) };
	const int windowX{ (screenWidth - windowWidth) / 2 + options.x };
	const int windowY{ (screenHeight - windowHeight) / 2 + options.y };

	handle = ::CreateWindowExW(
		options.styleEx,
		className,
		options.title.c_str(),
		options.style,
		windowX,
		windowY,
		windowWidth,
		windowHeight,
		nullptr,
		nullptr,
		instanceHandle,
		this);

	return nullptr != handle;
}

void Window::Release()
{
	if (nullptr != handle)
	{
		::DestroyWindow(handle);
		handle = nullptr;
	}
}

HWND Window::GetHandle() const noexcept
{
	return handle;
}

const std::wstring& Window::GetTitle() const noexcept
{
	return options.title;
}

void Window::SetTitle(const std::wstring& title_) noexcept
{
	if (nullptr != handle)
	{
		options.title = title_;
		::SetWindowTextW(handle, options.title.c_str());
	}
}

int Window::GetPositionX() const noexcept
{
	return options.x;
}

void Window::SetPositionX(int x_) noexcept
{
	if (nullptr != handle)
	{
		options.x = x_;
		::MoveWindow(handle, options.x, options.y, options.width, options.height, FALSE);
	}
}

int Window::GetPositionY() const noexcept
{
	return options.y;
}

void Window::SetPositionY(int y_) noexcept
{
	if (nullptr != handle)
	{
		options.y = y_;
		::MoveWindow(handle, options.x, options.y, options.width, options.height, FALSE);
	}
}

std::pair<int, int> Window::GetPosition() const noexcept
{
	return { options.x, options.y };
}

void Window::SetPosition(int x_, int y_) noexcept
{
	if (nullptr != handle)
	{
		options.x = x_;
		options.y = y_;
		::MoveWindow(handle, options.x, options.y, options.width, options.height, FALSE);
	}
}

int Window::GetWidth() const noexcept
{
	return options.width;
}

void Window::SetWidth(int width_) noexcept
{
	options.width = std::max(1, width_);
}

int Window::GetHeight() const noexcept
{
	return options.height;
}

void Window::SetHeight(int height_) noexcept
{
	options.height = std::max(1, height_);
}

std::pair<int, int> Window::GetSize() const noexcept
{
	return { options.width, options.height };
}

void Window::SetSize(int width_, int height_)
{
	if (nullptr != handle)
	{
		options.width = std::max(1, width_);
		options.height = std::max(1, height_);
		::MoveWindow(handle, 0, 0, options.width, options.height, FALSE);
	}
}

DWORD Window::GetStyle() const noexcept
{
	return options.style;
}

void Window::SetStyle(DWORD style_) noexcept
{
	if (handle != nullptr)
	{
		options.style = style_;
		::SetWindowLongPtrW(handle, GWL_STYLE, style_);
		::SetWindowPos(handle, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}
}

DWORD Window::GetStyleEx() const noexcept
{
	return options.styleEx;
}

void Window::SetStyleEx(DWORD styleEx_) noexcept
{
	if (handle != nullptr)
	{
		options.styleEx = styleEx_;
		::SetWindowLongPtrW(handle, GWL_EXSTYLE, styleEx_);
		::SetWindowPos(handle, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}
}

void Window::Show() const noexcept
{
	if (nullptr != handle)
	{
		::ShowWindow(handle, SW_SHOWDEFAULT);
		::UpdateWindow(handle);
	}
}

void Window::Hide() const noexcept
{
	if (nullptr != handle)
	{
		::ShowWindow(handle, SW_HIDE);
		::UpdateWindow(handle);
	}
}

bool Window::PollEvent(Event& event_)
{
	if (eventQueue.empty())
	{
		MSG message{};
		while (::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessageW(&message);
		}
	}

	if (eventQueue.empty())
	{
		return false;
	}

	event_ = eventQueue.front();
	eventQueue.pop();
	return true;
}

LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

LRESULT Window::OnEvent(UINT message_, WPARAM wParam_, LPARAM lParam_)
{
	switch (message_)
	{
		case WM_CLOSE:
		{
			Event event{};
			event.type = Event::Type::WindowClose;
			event.windowHandle = handle;
			eventQueue.push(event);
			return 0;
		}
		case WM_SIZE:
		{
			const int width{ LOWORD(lParam_) };
			const int height{ HIWORD(lParam_) };

			options.width = width;
			options.height = height;

			Event event{};
			event.type = Event::Type::WindowResize;
			event.windowHandle = handle;
			event.resize.width = width;
			event.resize.height = height;
			eventQueue.push(event);
			return 0;
		}
		case WM_KEYDOWN:
		{
			Event event{};
			event.type = Event::Type::KeyDown;
			event.windowHandle = handle;
			event.key.keyCode = (KeyCode)wParam_;
			eventQueue.push(event);
			return 0;
		}
		case WM_KEYUP:
		{
			Event event{};
			event.type = Event::Type::KeyUp;
			event.windowHandle = handle;
			event.key.keyCode = (KeyCode)wParam_;
			eventQueue.push(event);
			return 0;
		}
		case WM_SYSKEYDOWN:
		{
			const bool isAltDown{ 0 != (lParam_ & (1 << 29)) };
			if (isAltDown && VK_RETURN == wParam_)
			{
				Event event{};
				event.type = Event::Type::WindowFullscreenToggle;
				event.windowHandle = handle;
				eventQueue.push(event);
				return 0;
			}
			break;
		}
		case WM_MOUSEMOVE:
		{
			Event event{};
			event.type = Event::Type::MouseMove;
			event.windowHandle = handle;
			event.mouseMove.x = GET_X_LPARAM(lParam_);
			event.mouseMove.y = GET_Y_LPARAM(lParam_);
			eventQueue.push(event);
			return 0;
		}
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		{
			Event event{};
			event.type = Event::Type::MouseButtonDown;
			event.windowHandle = handle;
			event.mouseButton.button =
				WM_LBUTTONDOWN == message_ ? ButtonCode::Left :
				WM_RBUTTONDOWN == message_ ? ButtonCode::Right :
					ButtonCode::Middle;
			event.mouseButton.x = GET_X_LPARAM(lParam_);
			event.mouseButton.y = GET_Y_LPARAM(lParam_);
			eventQueue.push(event);
			return 0;
		}
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		{
			Event event{};
			event.type = Event::Type::MouseButtonUp;
			event.windowHandle = handle;
			event.mouseButton.button =
				WM_LBUTTONUP == message_ ? ButtonCode::Left :
				WM_RBUTTONUP == message_ ? ButtonCode::Right :
					ButtonCode::Middle;
			event.mouseButton.x = GET_X_LPARAM(lParam_);
			event.mouseButton.y = GET_Y_LPARAM(lParam_);
			eventQueue.push(event);
			return 0;
		}
		default:;
	}

	return ::DefWindowProcW(handle, message_, wParam_, lParam_);
}
