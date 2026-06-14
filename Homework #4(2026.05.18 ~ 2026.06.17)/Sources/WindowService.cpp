#include "Precompiled.h"
#include "WindowService.h"

namespace
{
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
			{
				return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
			}
		}

		// 절대 도달 못함.
		::std::unreachable();
	}

	[[nodiscard]] DWORD GetStyle(const WindowService::Options& options_)
	{
		DWORD style{ WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX };
		if (options_.isFullscreen)
		{
			style = WS_POPUP;
		}
		else if (!options_.isBorderless)
		{
			style = WS_OVERLAPPEDWINDOW | (options_.isResizable ? WS_THICKFRAME | WS_MAXIMIZEBOX : 0);
		}
		else if (options_.isResizable)
		{
			style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
		}

		return style;
	}

	// 일단 얘가 하는 일이 별로 없긴 함. 일관성을 위해 작성.
	[[nodiscard]] DWORD GetStyleEx(const WindowService::Options& options_)
	{
		// TODO: 지금 당장은 Style Ex 건드리는 설정이 없음.
		// 나중에 필요하면 채워놓을 것.
		return WS_EX_OVERLAPPEDWINDOW;
	}
}

bool WindowService::Initialize(const Options& options_)
{
	assert(options_.x != 0 || options_.y != 0);
	assert(options_.width > 0 || options_.height > 0);

	options = options_;

	// TODO: 화면 중앙 위치 가져와서 x, y만큼 이동한 위치에 생성하도록.
	int x{ CW_USEDEFAULT };
	int y{ CW_USEDEFAULT };
	int width{ options.width };
	int height{ options.height };

	DWORD style{ GetStyle(options) };
	DWORD styleEx{ GetStyleEx(options) };

	if (options.isFullscreen)
	{
		const HMONITOR monitor{ ::MonitorFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY) };
		MONITORINFO monitorInfo{ .cbSize = sizeof(MONITORINFO) };
		if (!::GetMonitorInfoW(monitor, &monitorInfo))
		{
			return false;
		}

		x = monitorInfo.rcMonitor.left;
		y = monitorInfo.rcMonitor.top;
		width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
		height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
	}
	else
	{
		RECT rect{ };
		rect.right = options.width;
		rect.bottom = options.height;

		if (!::AdjustWindowRectEx(
			&rect,
			style,
			FALSE,
			styleEx))
		{
			return false;
		}

		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	// 이거 WindowService 내 필드 사용하게끔 사용하게 바꾸기.
	hWnd = ::CreateWindowExW(
		styleEx,
		L"Framework Window Class",
		options.title.c_str(),
		style,
		x,
		y,
		width,
		height,
		nullptr,
		nullptr,
		::GetModuleHandleW(nullptr),
		nullptr);

	if (hWnd == nullptr)
	{
		return false;
	}

	if (!::IsWindowVisible(hWnd))
	{
		::ShowWindow(hWnd, SW_SHOW);
		::UpdateWindow(hWnd);
	}

	return true;
}

void WindowService::Terminate() noexcept
{
	if (hWnd != nullptr)
	{
		::DestroyWindow(hWnd);
		hWnd = nullptr;
	}
}

void WindowService::Show() noexcept
{
	if (hWnd != nullptr)
	{
		::ShowWindow(hWnd, SW_SHOW);
	}
}

void WindowService::Hide() noexcept
{
	if (hWnd != nullptr)
	{
		::ShowWindow(hWnd, SW_HIDE);
	}
}

HWND WindowService::GetHWND() const noexcept
{
	return hWnd;
}

HINSTANCE WindowService::GetHINSTANCE() const noexcept
{
	return hInstance;
}

const std::wstring& WindowService::GetTitle() const noexcept
{
	return options.title;
}

void WindowService::SetTitle(std::wstring_view title_) noexcept
{
	if (hWnd != nullptr)
	{
		options.title = std::move(title_);
		::SetWindowTextW(hWnd, options.title.data());
	}
}

int WindowService::GetWidth() const noexcept
{
	return options.width;
}

void WindowService::SetWidth(int width_) noexcept
{
	SetSize(SIZE{ width_, options.height });
}

int WindowService::GetHeight() const noexcept
{
	return options.height;
}

void WindowService::SetHeight(int height_) noexcept
{
	SetSize(SIZE{ options.width, height_ });
}

SIZE WindowService::GetSize() const noexcept
{
	return SIZE{ options.width, options.height };
}

void WindowService::SetSize(SIZE size_) noexcept
{
	assert(width_ > 0 || height > 0);

	options.width = size_.cx;
	options.height = size_.cy;

	if (hWnd != nullptr)
	{
		RECT rect{};
		rect.right = options.width;
		rect.bottom = options.height;

		DWORD style{ GetStyle(options) };
		DWORD styleEx{ GetStyleEx(options) };

		if (::AdjustWindowRectEx(
			&rect,
			style,
			FALSE,
			styleEx))
		{
			::SetWindowPos(
				hWnd,
				nullptr,
				0,
				0,
				rect.right - rect.left,
				rect.bottom - rect.top,
				SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}
}

POINT WindowService::GetPosition() const noexcept
{
	if (hWnd == nullptr)
	{
		return POINT{};
	}

	RECT rect{};
	if (!::GetWindowRect(hWnd, &rect))
	{
		return POINT{};
	}

	return POINT{ rect.left, rect.top };
}

int WindowService::GetX() const noexcept
{
	return GetPosition().x;
}

void WindowService::SetX(int x_) noexcept
{
	SetPosition(POINT{ x_, GetY() });
}

int WindowService::GetY() const noexcept
{
	return GetPosition().y;
}

void WindowService::SetY(int y_) noexcept
{
	SetPosition(POINT{ GetX(), y_ });
}

void WindowService::SetPosition(POINT position_) noexcept
{
	assert(position_.x != 0 || position_.y != 0);

	if (hWnd != nullptr)
	{
		options.x = position_.x;
		options.y = position_.y;

		::SetWindowPos(
			hWnd,
			nullptr,
			options.x,
			options.y,
			0,
			0,
			SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

bool WindowService::IsFullscreen() const noexcept
{
	return options.isFullscreen;
}

void WindowService::SetFullscreen(bool fullscreen_) noexcept
{
	if (hWnd != nullptr)
	{
		options.isFullscreen = fullscreen_;

		DWORD style{ GetStyle(options) };
		::SetWindowLongPtrW(
			hWnd,
			GWL_STYLE,
			static_cast<LONG_PTR>(style));

		DWORD styleEx{ GetStyleEx(options) };
		::SetWindowLongPtrW(
			hWnd,
			GWL_EXSTYLE,
			static_cast<LONG_PTR>(styleEx));

		::SetWindowPos(
			hWnd,
			nullptr,
			0,
			0,
			0,
			0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}
}

bool WindowService::IsResizable() const noexcept
{
	return options.isResizable;
}

void WindowService::SetResizable(bool resizable_) noexcept
{
	if (hWnd != nullptr)
	{
		options.isResizable = resizable_;

		DWORD style{ GetStyle(options) };
		::SetWindowLongPtrW(
			hWnd,
			GWL_STYLE,
			static_cast<LONG_PTR>(style));

		DWORD styleEx{ GetStyleEx(options) };
		::SetWindowLongPtrW(
			hWnd,
			GWL_EXSTYLE,
			static_cast<LONG_PTR>(styleEx));

		::SetWindowPos(
			hWnd,
			nullptr,
			0,
			0,
			0,
			0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}
}

bool WindowService::IsBorderless() const noexcept
{
	return options.isBorderless;
}

void WindowService::SetBorderless(bool decorated_) noexcept
{
	if (hWnd != nullptr)
	{
		options.isBorderless = decorated_;

		DWORD style{ GetStyle(options) };
		::SetWindowLongPtrW(
			hWnd,
			GWL_STYLE,
			static_cast<LONG_PTR>(style));

		DWORD styleEx{ GetStyleEx(options) };
		::SetWindowLongPtrW(
			hWnd,
			GWL_EXSTYLE,
			static_cast<LONG_PTR>(styleEx));

		::SetWindowPos(
			hWnd,
			nullptr,
			0,
			0,
			0,
			0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}
}

bool WindowService::IsMinimized() const noexcept
{
	return hWnd != nullptr && ::IsIconic(hWnd);
}

void WindowService::Minimize() noexcept
{
	if (hWnd != nullptr)
	{
		::ShowWindow(hWnd, SW_MINIMIZE);
	}
}

bool WindowService::IsMaximized() const noexcept
{
	return hWnd != nullptr && ::IsZoomed(hWnd);
}

void WindowService::Maximize() noexcept
{
	if (hWnd != nullptr)
	{
		::ShowWindow(hWnd, SW_MAXIMIZE);
	}
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
	wc.lpszClassName = L"Framework Window Class";
	wc.hIconSm = ::LoadIconW(hInstance, IDI_APPLICATION);
	::RegisterClassExW(&wc);
}

void WindowService::OnRemove()
{
	::UnregisterClassW(L"Framework Window Class", hInstance);
	hInstance = nullptr;
}
